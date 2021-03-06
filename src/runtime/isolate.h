// Copyright 2017-2018 VMware, Inc.
// SPDX-License-Identifier: BSD-2-Clause
//
// The BSD-2 license (the License) set forth below applies to all parts of the
// Cascade project.  You may not use this file except in compliance with the
// License.
//
// BSD-2 License
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef CASCADE_SRC_RUNTIME_ISOLATE_H
#define CASCADE_SRC_RUNTIME_ISOLATE_H

#include <unordered_map>
#include "src/runtime/ids.h"
#include "src/verilog/ast/ast_fwd.h"
#include "src/verilog/ast/visitors/builder.h"

namespace cascade {

class DataPlane;

class Isolate : public Builder {
  public:
    // Constructors:
    Isolate(const DataPlane* dp);
    ~Isolate() override = default;

    // Deterministically tranforms a program variable into a globally-unique
    // variable ID.
    VId isolate(const Identifier* id);
    // Transforms an instantiated module from a program into a stand-alone
    // declaration with equivalent semantics.
    ModuleDeclaration* isolate(const ModuleDeclaration* src, int ignore);

  private:
    // Runtime State:
    const DataPlane* dp_;

    // Compilation State:
    const ModuleDeclaration* src_;
    int ignore_;
    std::unordered_map<const Identifier*, size_t> symbol_table_;
    std::unordered_map<const Identifier*, size_t> module_table_;
  
    // Builder Interface:
    Attributes* build(const Attributes* as) override;
    Expression* build(const Identifier* i) override;
    ModuleDeclaration* build(const ModuleDeclaration* md) override; 
    ModuleItem* build(const InitialConstruct* ic) override;
    ModuleItem* build(const GenvarDeclaration* gd) override;
    ModuleItem* build(const IntegerDeclaration* id) override;
    ModuleItem* build(const LocalparamDeclaration* ld) override;
    ModuleItem* build(const ParameterDeclaration* pd) override;
    ModuleItem* build(const RegDeclaration* rd) override;
    ModuleItem* build(const PortDeclaration* pd) override;

    // Returns a mangled identifier
    Identifier* to_mangled_id(const Identifier* id);
    // Returns a new locally unique identifier
    Identifier* to_local_id(const Identifier* id);
    // Returns a copy of the global identifier that corresponds to this id
    Identifier* to_global_id(const Identifier* id);

    // Returns a module declaration with a mangled id and global io ports
    ModuleDeclaration* get_shell();
    // Generates a list of declarations for local variables
    Many<ModuleItem>* get_local_decls();
    // Recursively process a list of items
    Many<ModuleItem>* get_items(const Many<ModuleItem>* mis, bool top_level);

    // Replaces an instantiation with assignments
    void replace(Many<ModuleItem>* res, const ModuleInstantiation* mi);
    // Flattens the elaborated branch of a case generate construct
    void flatten(Many<ModuleItem>* res, const CaseGenerateConstruct* cgc);
    // Flattens the elaborated branch of an if generate construct
    void flatten(Many<ModuleItem>* res, const IfGenerateConstruct* igc);
    // Flattens the elaborated branches of a loop generate construct
    void flatten(Many<ModuleItem>* res, const LoopGenerateConstruct* lgc);
    // Flattens a generate block
    void flatten(Many<ModuleItem>* res, const GenerateBlock* gb);
    // Flattens a generate region
    void flatten(Many<ModuleItem>* res, const GenerateRegion* gr);
};

} // namespace cascade

#endif
