#ifndef CPPAD_CG_LLVM_INCLUDED
#define CPPAD_CG_LLVM_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2014 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Eclipse Public License Version 1.0 (EPL1), and
 *   - GNU General Public License Version 3 (GPL3).
 *
 *  EPL1 terms and conditions can be found in the file "epl-v10.txt", while
 *  terms and conditions for the GPL3 can be found in the file "gpl3.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */

#ifdef LLVM_VERSION_MAJOR

#if LLVM_VERSION_MAJOR==3 && LLVM_VERSION_MINOR==2
#include <cppadcg/model/llvm/v3_2/cg_llvm3_2.hpp>
#elif LLVM_VERSION_MAJOR==3 && (LLVM_VERSION_MINOR==3 || LLVM_VERSION_MINOR==4)
#include <cppadcg/model/llvm/v3_4/cg_llvm3_4.hpp>
#endif

#endif

#endif