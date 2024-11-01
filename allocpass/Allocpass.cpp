#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {

struct AllocSizePass : public PassInfoMixin<AllocSizePass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        // Get the data layout to calculate sizes
        const DataLayout &DL = M.getDataLayout();
        LLVMContext &Context = M.getContext();

        FunctionCallee printfFunc = M.getOrInsertFunction(
            "printf", FunctionType::get(IntegerType::getInt32Ty(Context), 
            PointerType::get(Type::getInt8Ty(Context), 0), true)
        );

        Constant *formatStr = ConstantDataArray::getString(Context, "Allocation size: %lu bytes\n");

        GlobalVariable *formatStrVar = new GlobalVariable(M, formatStr->getType(), true,
                                                          GlobalValue::PrivateLinkage, formatStr);

        for (auto &F : M.functions()) {
            errs() << "Function: " << F.getName() << "\n";
            for (auto &BB : F) {
                errs() << "  Basic Block: ";
                BB.printAsOperand(errs(), false);
                errs() << "\n";

                for (auto &I : BB) {
                    if (auto *allocaInst = dyn_cast<AllocaInst>(&I)) {
                        Type *allocType = allocaInst->getAllocatedType();
                        uint64_t size = DL.getTypeAllocSize(allocType);
                        errs() << "    Allocation of type " << *allocType
                               << " with size: " << size << " bytes\n";

                        IRBuilder<> builder(allocaInst->getNextNode());

                        Value *formatStrPtr = builder.CreatePointerCast(formatStrVar, 
                                                                       PointerType::get(Type::getInt8Ty(Context), 0));

                        Value *sizeVal = ConstantInt::get(Type::getInt64Ty(Context), size);
                        builder.CreateCall(printfFunc, {formatStrPtr, sizeVal});
                    }
                }
            }
        }
        return PreservedAnalyses::all();
    }
};

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "Allocation Size Pass with Print",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(AllocSizePass());
                });
        }
    };
}
