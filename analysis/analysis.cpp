#include <chrono>
#include <exception>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

class Analysis : public FunctionPass {
public:
  using set = std::unordered_set<std::string>;
  using map = std::unordered_map<std::string, set>;

  Analysis() : FunctionPass(ID), counter(0) {
    BuildCallGraph();
    sleepable = ConnectedComponents({"__might_sleep"});
    atomic = ConnectedComponents(entering_atomic);
  }

  virtual bool runOnFunction(Function &F) override {
    std::string name = F.getName().str();
    if (sleepable.find(name) == sleepable.end()) {
      return false;
    }
    if (atomic.find(name) == atomic.end()) {
      return false;
    }

    start = std::chrono::steady_clock::now();

    BasicBlock &entry = F.getEntryBlock();
    if (DepthFirstSearch(&entry, 0)) {
      errs() << "Potential bug: " << name << "\n";
    }

    visited.clear();

    return false;
  }

  void BuildCallGraph() {
    const std::string filename("allfunctions.txt");

    std::string caller;
    std::string callee;

    std::ifstream ifs(filename);

    while (!ifs.eof()) {
      std::string function;
      ifs >> function;
      if (function.front() == '*') {
        caller = function.substr(1);
      } else if (function.front() == '>') {
        callee = function.substr(1);

        caller2callee[caller].insert(callee);
        callee2caller[callee].insert(caller);
      }
    }

    ifs.close();
  }

  set ConnectedComponents(const set &initial) {
    set ret(initial);

    set::size_type size = 0;
    while (ret.size() > size) {
      size = ret.size();

      set copy(ret);
      for (auto &function : copy) {
        ret.insert(callee2caller[function].cbegin(),
                   callee2caller[function].cend());
      }
    }

    return ret;
  }

  bool DepthFirstSearch(BasicBlock *BB, int counter) {
    auto inserted = visited.insert(BB);

    bool buggy = false;

    for (Instruction &I : BB->getInstList()) {
      if (auto CI = dyn_cast<CallInst>(&I)) {
        Function *called = CI->getCalledFunction();
        if (called == nullptr) {
          continue;
        }
        std::string name = called->getName().str();
        if (entering_atomic.find(name) != entering_atomic.end()) {
          counter++;
        } else if (leaving_atomic.find(name) != leaving_atomic.end()) {
          counter--;
        } else if (counter > 0 && sleepable.find(name) != sleepable.end()) {
          // errs() << "Potential bug: " << I.getFunction()->getName() << "\n";
          buggy = true;
        }
      }
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    if (diff.count() < 10) {
      for (BasicBlock *Succ : successors(BB)) {
        if (!buggy && Succ && visited.find(Succ) == visited.end()) {
          if (DepthFirstSearch(Succ, counter)) {
            buggy = true;
          }
        }
      }
    }

    visited.erase(inserted.first);

    return buggy;
  }

  static char ID;

private:
  map caller2callee;
  map callee2caller;

  set sleepable;
  set atomic;
  std::unordered_set<BasicBlock *> visited;

  std::chrono::time_point<std::chrono::steady_clock> start;

  const static set entering_atomic;
  const static set leaving_atomic;

  int counter;
};

char Analysis::ID = 0;

const Analysis::set Analysis::entering_atomic({"preempt_count_add"});
const Analysis::set Analysis::leaving_atomic({"preempt_count_sub"});

static RegisterPass<Analysis> X("analysis", "Analysis w/ bitcode",
                                false /* Only looks at CFG */,
                                false /* Analysis Pass */);
