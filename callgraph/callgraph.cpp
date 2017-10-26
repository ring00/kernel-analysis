#include <fstream>
#include <set>
#include <string>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetLowering.h>

using namespace llvm;

#define for_each_inst(F, I)                                                    \
  for (auto &_BB : (F).getBasicBlockList())                                    \
    for (auto &I : _BB.getInstList())

class Callgraph : public FunctionPass {
public:
  Callgraph() : FunctionPass(ID), ofs(filename) {}

  virtual bool runOnFunction(Function &F) override {
    std::set<std::string> names;
    for_each_inst(F, I) {
      if (auto CI = dyn_cast<CallInst>(&I)) {
        Function *called = CI->getCalledFunction();
        if (called && !called->getName().startswith("llvm")) {
          names.insert(called->getName().str());
        }
      }
    }

    if (names.size() > 0) {
      ofs << "*" << F.getName().str() << std::endl;
      for (auto &name : names) {
        ofs << ">" << name << std::endl;
      }
    }
    return true;
  }

  static const std::string filename;

  static char ID;

private:
  std::ofstream ofs;
};

const std::string Callgraph::filename = "allfunctions.txt";

char Callgraph::ID = 0;

static RegisterPass<Callgraph> X("callgraph", "Generate callgraph w/ bitcode",
                                 false /* Only looks at CFG */,
                                 false /* Analysis Pass */);
