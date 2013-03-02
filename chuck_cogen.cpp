/*----------------------------------------------------------------------------
 ChucK Concurrent, On-the-fly Audio Programming Language
 Compiler and Virtual Machine
 
 Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
 http://chuck.cs.princeton.edu/
 http://soundlab.cs.princeton.edu/
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 U.S.A.
 -----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck_cogen.h
// desc: ...
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Spring 2013
//-----------------------------------------------------------------------------

#include "chuck_cogen.h"

#ifdef __MACOSX_CORE__

#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/FrontendOptions.h>

#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Module.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <fstream>

using namespace std;


class Chuck_ClangCogenGraph : public Chuck_CogenGraph
{
public:
    
    Chuck_ClangCogenGraph(llvm::Module *_Module)
    {
        tickFn = _Module->getFunction("tick");
        
        std::string Error;
        EE.reset(llvm::ExecutionEngine::createJIT(_Module, &Error));
        if(!EE)
        {
            llvm::errs() << "unable to make execution engine: " << Error << "\n";
        }
    }
    
    t_CKINT tick(SAMPLE input, SAMPLE &output)
    {
        vector<llvm::GenericValue> args;
        EE->runFunction(tickFn, args);
        return 0;
    }
    
private:
    
    llvm::OwningPtr<llvm::ExecutionEngine> EE;
    llvm::Function *tickFn;
};

class Chuck_ClangCogen : public Chuck_Cogen
{
public:
    Chuck_ClangCogen()
    {
        llvm::InitializeNativeTarget();
    }
    
    ~Chuck_ClangCogen()
    {
        llvm::llvm_shutdown();
    }
    
    Chuck_CogenGraph * cogenerate(Chuck_UGen * root)
    {
        string code = "#include <stdio.h>\nextern \"C\" void tick() { printf(\"tick\\n\"); }\n";
        
        // generate code
        
        // compile code
        clang::CodeGenAction * act = compile(code);
        
        return new Chuck_ClangCogenGraph(act->takeModule());
    }
    
private:
    
    clang::CodeGenAction * compile(string code)
    {
        char tmpPath[256];
        tmpnam(tmpPath);
        
        ofstream sourceFile(tmpPath);
        
        sourceFile << code;
        
        sourceFile.close();
        
        // Arguments to pass to the clang frontend
        vector<const char *> args;
             args.push_back(tmpPath);
        args.push_back("-x");
        args.push_back("c++");
        
        // The compiler invocation needs a DiagnosticsEngine so it can report problems
        clang::DiagnosticOptions * opts = new clang::DiagnosticOptions();
        clang::TextDiagnosticPrinter *DiagClient = new clang::TextDiagnosticPrinter(llvm::errs(), opts);
        llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagID(new clang::DiagnosticIDs());
        clang::DiagnosticsEngine Diags(DiagID, opts, DiagClient);
        
        // Create the compiler invocation
        llvm::OwningPtr<clang::CompilerInvocation> CI(new clang::CompilerInvocation);
        clang::CompilerInvocation::CreateFromArgs(*CI, &args[0], &args[0] + args.size(), Diags);
        
        // Create the compiler instance
        clang::CompilerInstance Clang;
        Clang.setInvocation(CI.take());
        
        // Get ready to report problems
        Clang.createDiagnostics(args.size(), &args[0]);
        if (!Clang.hasDiagnostics())
            return NULL;
        
        // Create an action and make the compiler instance carry it out
        clang::CodeGenAction *Act = new clang::EmitLLVMOnlyAction();
        if (!Clang.ExecuteAction(*Act))
            return NULL;
        
        unlink(tmpPath);
        
        return Act;
    }
};


Chuck_Cogen * Chuck_Cogen::defaultCogenerator()
{
    static Chuck_Cogen * _defaultCogenerator = new Chuck_ClangCogen();
    
    return _defaultCogenerator;
}


#else // def __MACOSX_CORE__

Chuck_Cogen * Chuck_Cogen::defaultCogenerator()
{
    return NULL;
}

#endif // def __MACOSX_CORE__
