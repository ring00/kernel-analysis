#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

class Analysis : public FunctionPass {
   public:
    Analysis() : FunctionPass(ID), sleepable({"__might_sleep"}) {
        const std::string filename("allfunctions.txt");

        std::ifstream ifs(filename);

        std::string caller;
        std::string callee;

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

        std::unordered_set<std::string>::size_type size = 0;
        while (sleepable.size() > size) {
            size = sleepable.size();

            std::unordered_set<std::string> copy(sleepable);

            for (auto& function : copy) {
                sleepable.insert(callee2caller[function].cbegin(),
                                 callee2caller[function].cend());
            }
        }
    }

    virtual bool runOnFunction(Function& F) override {
        StringRef name = F.getName();
        if (sleepable.find(name.str()) == sleepable.end()) {
            return false;
        }
        errs() << name << "\n";
        return false;
    }

    static char ID;

   private:
    using set = std::unordered_set<std::string>;
    using map = std::unordered_map<std::string, set>;

    map caller2callee;
    map callee2caller;
    set sleepable;
};

char Analysis::ID = 0;

static RegisterPass<Analysis> X("analysis", "Analysis w/ bitcode",
                                false /* Only looks at CFG */,
                                false /* Analysis Pass */);
