#ifndef CPPAD_CG_C_LANG_COMPILE_MODEL_HELPER_FOR0_INCLUDED
#define CPPAD_CG_C_LANG_COMPILE_MODEL_HELPER_FOR0_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
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

namespace CppAD {

    template<class Base>
    void CLangCompileModelHelper<Base>::generateZeroSource(std::map<std::string, std::string>& sources) {
        const std::string jobName = "model (zero-order forward)";

        startingJob("operation graph for '" + jobName + "'");

        CodeHandler<Base> handler;
        handler.setJobTimer(this);

        vector<CGBase> indVars(_fun.Domain());
        handler.makeVariables(indVars);
        if (_x.size() > 0) {
            for (size_t i = 0; i < indVars.size(); i++) {
                indVars[i].setValue(_x[i]);
            }
        }

        vector<CGBase> dep;

        if (_loopTapes.empty()) {
            dep = _fun.Forward(0, indVars);
        } else {
            /**
             * Contains loops
             */
            dep = prepareForward0WithLoops(handler, indVars);
        }

        finishedJob();

        CLanguage<Base> langC(_baseTypeName);
        langC.setMaxAssigmentsPerFunction(_maxAssignPerFunc, &sources);
        langC.setGenerateFunction(_name + "_" + FUNCTION_FORWAD_ZERO);

        std::ostringstream code;
        std::auto_ptr<VariableNameGenerator<Base> > nameGen(createVariableNameGenerator());

        handler.generateCode(code, langC, dep, *nameGen, _atomicFunctions, jobName);
    }
    
}

#endif