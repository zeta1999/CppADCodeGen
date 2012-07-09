#ifndef CPPAD_CG_C_LANGUAGE_INCLUDED
#define	CPPAD_CG_C_LANGUAGE_INCLUDED
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2012 Ciengis

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

#define CPPAD_CG_C_LANG_FUNCNAME(fn) \
inline virtual const std::string& fn ## FuncName() {\
    static const std::string name(#fn);\
    return name;\
}

namespace CppAD {

    /**
     * Generates code for the C language
     * 
     * \author Joao Leal
     */
    template<class Base>
    class CLanguage : public Language<Base> {
    protected:
        static const std::string _C_COMP_OP_LT;
        static const std::string _C_COMP_OP_LE;
        static const std::string _C_COMP_OP_EQ;
        static const std::string _C_COMP_OP_GE;
        static const std::string _C_COMP_OP_GT;
        static const std::string _C_COMP_OP_NE;

    protected:
        // line indentation
        const std::string _spaces;
        // output stream for the generated source code
        std::ostream* _out;
        // creates the variable names
        VariableNameGenerator<Base>* _nameGen;
        // auxiliary string stream
        std::stringstream _ss;
        //
        size_t _independentSize;
        //
        size_t _minTemporaryVarID;
        // maps the variable IDs to the their position in the dependent vector
        // (some IDs may be the same as the independent variables when dep = indep)
        std::map<size_t, size_t> _dependentIDs;
        // the dependent variable vector
        const std::vector<CG<Base> >* _dependent;
        // the temporary variables that may require a declaration
        std::map<size_t, SourceCodeFragment<Base>*> _temporary;
        // the operator used for assignment of dependent variables
        std::string _depAssignOperation;
        // whether or not to ignore assignment of constant zero values to dependent variables
        bool _ignoreZeroDepAssign;
    public:

        CLanguage(size_t spaces = 3) :
            _spaces(spaces, ' '),
            _out(NULL),
            _nameGen(NULL),
            _independentSize(0),
            _dependent(NULL),
            _depAssignOperation("="),
            _ignoreZeroDepAssign(false) {
        }

        inline const std::string& getDependentAssignOperation() const {
            return _depAssignOperation;
        }

        inline void setDependentAssignOperation(const std::string& depAssignOperation) {
            _depAssignOperation = depAssignOperation;
        }

        inline bool isIgnoreZeroDepAssign() const {
            return _depAssignOperation;
        }

        inline void setIgnoreZeroDepAssign(bool ignore) {
            _ignoreZeroDepAssign = ignore;
        }

        virtual std::string generateTemporaryVariableDeclaration(const std::string& varTypeName) {
            assert(_nameGen != NULL);

            std::string code;

            // declare variables
            if (_temporary.size() > 0) {
                typename std::map<size_t, SourceCodeFragment<Base>*>::const_iterator it;

                for (it = _temporary.begin(); it != _temporary.end(); ++it) {
                    SourceCodeFragment<Base>* var = it->second;
                    if (var->getName() == NULL) {
                        var->setName(_nameGen->generateTemporary(*var));
                    }
                }

                SourceCodeFragment<Base>* var1 = _temporary.begin()->second;
                const std::string& varName1 = *var1->getName();
                code = _spaces + varTypeName + " " + varName1;

                // use a crude estimate of the string size to reserve space
                code.reserve(varTypeName.size() + 3 + _temporary.size() *(varName1.size() + 3));

                it = _temporary.begin();
                for (it++; it != _temporary.end(); ++it) {
                    code += ", " + *it->second->getName();
                }
                code += ";\n";
            }

            return code;
        }

        CPPAD_CG_C_LANG_FUNCNAME(abs)
        CPPAD_CG_C_LANG_FUNCNAME(acos)
        CPPAD_CG_C_LANG_FUNCNAME(asin)
        CPPAD_CG_C_LANG_FUNCNAME(atan)
        CPPAD_CG_C_LANG_FUNCNAME(cosh)
        CPPAD_CG_C_LANG_FUNCNAME(cos)
        CPPAD_CG_C_LANG_FUNCNAME(exp)
        CPPAD_CG_C_LANG_FUNCNAME(log)
        CPPAD_CG_C_LANG_FUNCNAME(sinh)
        CPPAD_CG_C_LANG_FUNCNAME(sin)
        CPPAD_CG_C_LANG_FUNCNAME(sqrt)
        CPPAD_CG_C_LANG_FUNCNAME(tanh)
        CPPAD_CG_C_LANG_FUNCNAME(tan)
        CPPAD_CG_C_LANG_FUNCNAME(pow)

    protected:

        inline std::ostream& out() {
            return *_out;
        }

        virtual void generateSourceCode(std::ostream& out, LanguageGenerationData<Base>& info) {
            _out = &out;
            _independentSize = info.independent.size();
            _dependent = &info.dependent;
            _nameGen = &info.nameGen;
            _minTemporaryVarID = info.minTemporaryVarID;

            const std::vector<CG<Base> >& dependent = info.dependent;
            const std::vector<SourceCodeFragment<Base>*>& variableOrder = info.variableOrder;

            _temporary.clear();

            //generate names for the independent variables
            typename std::vector<SourceCodeFragment<Base> *>::const_iterator it;
            for (it = info.independent.begin(); it != info.independent.end(); ++it) {
                SourceCodeFragment<Base>& op = **it;
                if (op.getName() == NULL) {
                    op.setName(_nameGen->generateIndependent(op));
                }
            }

            // generate names for the dependent variables (must be after naming the independent)
            for (size_t i = 0; i < dependent.size(); i++) {
                SourceCodeFragment<Base>* op = dependent[i].getSourceCodeFragment();
                if (op != NULL && op->getName() == NULL) {
                    op->setName(_nameGen->generateDependent(dependent[i], i));
                }
            }

            // dependent variables indexes that are copies of other dependent variables
            std::set<size_t> dependentDuplicates;

            for (size_t i = 0; i < dependent.size(); i++) {
                SourceCodeFragment<Base>* op = dependent[i].getSourceCodeFragment();
                if (op != NULL && op->operation() != CGInvOp) {
                    size_t varID = op->variableID();
                    if (varID > 0) {
                        std::map<size_t, size_t>::const_iterator it2 = _dependentIDs.find(varID);
                        if (it2 == _dependentIDs.end()) {
                            _dependentIDs[op->variableID()] = i;
                        } else {
                            // there can be several dependent variables with the same ID
                            dependentDuplicates.insert(i);
                        }
                    }
                }
            }

            if (variableOrder.size() > 0) {
                // non-constant variables
                *_out << _spaces << "// expressions: " << variableOrder.size() << "\n";

                // generate names for temporary variables
                for (it = variableOrder.begin(); it != variableOrder.end(); ++it) {
                    SourceCodeFragment<Base>& op = **it;
                    if (op.getName() == NULL) {
                        if (!isDependent(op)) {
                            op.setName(_nameGen->generateTemporary(op));
                        }
                    }
                }

                for (it = variableOrder.begin(); it != variableOrder.end(); ++it) {
                    SourceCodeFragment<Base>& op = **it;

                    bool isDep = isDependent(op);
                    if (!isDep) {
                        _temporary[op.variableID()] = &op;
                    }

                    bool condAssign = isCondAssign(op.operation());
                    if (!condAssign) {
                        *_out << _spaces << createVariableName(op) << " ";
                        *_out << (isDep ? _depAssignOperation : "=") << " ";
                    }
                    printExpressionNoVarCheck(op);
                    if (!condAssign) {
                        *_out << ";\n";
                    }
                }
            }

            // dependent duplicates
            if (dependentDuplicates.size() > 0) {
                *_out << _spaces << "// variable duplicates: " << dependentDuplicates.size() << "\n";
                for (std::set<size_t>::const_iterator it = dependentDuplicates.begin(); it != dependentDuplicates.end(); ++it) {
                    size_t index = *it;
                    const CG<Base>& dep = (*_dependent)[index];
                    std::string varName = _nameGen->generateDependent(dep, index);
                    const std::string& origVarName = *dep.getSourceCodeFragment()->getName();

                    *_out << _spaces << varName << " " << _depAssignOperation << " " << origVarName << ";\n";
                }
            }

            // constant dependent variables 
            *_out << _spaces << "// dependent variables without operations\n";
            for (size_t i = 0; i < dependent.size(); i++) {
                if (dependent[i].isParameter()) {
                    if (!_ignoreZeroDepAssign || !dependent[i].IdenticalZero()) {
                        std::string varName = _nameGen->generateDependent(dependent[i], i);
                        *_out << _spaces << varName << " " << _depAssignOperation << " ";
                        printParameter(dependent[i].getParameterValue());
                        *_out << ";\n";
                    }
                } else if (dependent[i].getSourceCodeFragment()->operation() == CGInvOp) {
                    std::string varName = _nameGen->generateDependent(dependent[i], i);
                    const std::string& indepName = *dependent[i].getSourceCodeFragment()->getName();
                    *_out << _spaces << varName << " " << _depAssignOperation << " " << indepName << ";\n";
                }
            }
        }

        virtual bool createsNewVariable(const SourceCodeFragment<Base>& op) {
            return op.totalUsageCount() > 1 ||
                    op.operation() == CGComOpLt ||
                    op.operation() == CGComOpLe ||
                    op.operation() == CGComOpEq ||
                    op.operation() == CGComOpGe ||
                    op.operation() == CGComOpGt ||
                    op.operation() == CGComOpNe;
        }

        virtual bool requiresVariableArgument(enum CGOpCode op, size_t argIndex) {
            return op == CGSignOp;
        }

        inline const std::string& createVariableName(SourceCodeFragment<Base>& var) {
            assert(var.variableID() > 0);

            if (var.getName() == NULL) {
                if (var.variableID() <= _independentSize) {
                    var.setName(_nameGen->generateIndependent(var));

                } else if (var.variableID() < _minTemporaryVarID) {
                    std::map<size_t, size_t>::const_iterator it = _dependentIDs.find(var.variableID());
                    assert(it != _dependentIDs.end());

                    size_t index = it->second;
                    const CG<Base>& dep = (*_dependent)[index];
                    var.setName(_nameGen->generateDependent(dep, index));

                } else {
                    var.setName(_nameGen->generateTemporary(var));
                }
            }

            return *var.getName();
        }

        virtual void printIndependentVariableName(SourceCodeFragment<Base>& op) {
            assert(op.arguments().size() == 0);

            out() << _nameGen->generateIndependent(op);
        }

        virtual void print(const Argument<Base>& arg) {
            if (arg.operation() != NULL) {
                // expression
                printExpression(*arg.operation());
            } else {
                // parameter
                printParameter(*arg.parameter());
            }
        }

        virtual void printExpression(SourceCodeFragment<Base>& op) throw (CGException) {
            if (op.variableID() > 0) {
                out() << createVariableName(op);
                return;
            }

            printExpressionNoVarCheck(op);
        }

        virtual void printExpressionNoVarCheck(SourceCodeFragment<Base>& op) throw (CGException) {
            switch (op.operation()) {
                case CGAbsOp:
                case CGAcosOp:
                case CGAsinOp:
                case CGAtanOp:
                case CGCoshOp:
                case CGCosOp:
                case CGExpOp:
                case CGLogOp:
                case CGSinhOp:
                case CGSinOp:
                case CGSqrtOp:
                case CGTanhOp:
                case CGTanOp:
                    printUnaryFunction(op);
                    break;
                case CGAddOp:
                    printOperationAdd(op);
                    break;
                case CGComOpLt:
                case CGComOpLe:
                case CGComOpEq:
                case CGComOpGe:
                case CGComOpGt:
                case CGComOpNe:
                    printConditionalAssignment(op);
                    break;
                case CGDivOp:
                    printOperationDiv(op);
                    break;
                case CGInvOp:
                    printIndependentVariableName(op);
                    break;
                case CGMulOp:
                    printOperationMul(op);
                    break;
                case CGPowOp:
                    printPowFunction(op);
                    break;
                case CGSignOp:
                    printSignFunction(op);
                    break;
                case CGSubOp:
                    printOperationMinus(op);
                    break;

                case CGUnMinusOp:
                    printOperationUnaryMinus(op);
                    break;
                default:
                    std::stringstream ss;
                    ss << "Unkown operation code '" << op.operation() << "'.";
                    throw CGException(ss.str());
            }
        }

        virtual void printUnaryFunction(SourceCodeFragment<Base>& op) throw (CGException) {
            assert(op.arguments().size() == 1);

            switch (op.operation()) {
                case CGAbsOp:
                    out() << absFuncName();
                    break;
                case CGAcosOp:
                    out() << acosFuncName();
                    break;
                case CGAsinOp:
                    out() << asinFuncName();
                    break;
                case CGAtanOp:
                    out() << atanFuncName();
                    break;
                case CGCoshOp:
                    out() << coshFuncName();
                    break;
                case CGCosOp:
                    out() << cosFuncName();
                    break;
                case CGExpOp:
                    out() << expFuncName();
                    break;
                case CGLogOp:
                    out() << logFuncName();
                    break;
                case CGSinhOp:
                    out() << sinhFuncName();
                    break;
                case CGSinOp:
                    out() << sinFuncName();
                    break;
                case CGSqrtOp:
                    out() << sqrtFuncName();
                    break;
                case CGTanhOp:
                    out() << tanhFuncName();
                    break;
                case CGTanOp:
                    out() << tanFuncName();
                    break;
                default:
                    std::stringstream ss;
                    ss << "Unkown function name for operation code '" << op.operation() << "'.";
                    throw CGException(ss.str());
            }

            out() << "(";
            print(op.arguments()[0]);
            out() << ")";
        }

        virtual void printPowFunction(SourceCodeFragment<Base>& op) throw (CGException) {
            assert(op.arguments().size() == 2);

            out() << powFuncName() << "(";
            print(op.arguments()[0]);
            out() << ", ";
            print(op.arguments()[1]);
            out() << ")";
        }

        virtual void printSignFunction(SourceCodeFragment<Base>& op) throw (CGException) {
            assert(op.arguments().size() == 1);
            assert(op.arguments()[0].operation() != NULL);
            assert(op.arguments()[0].operation()->variableID() > 0);

            SourceCodeFragment<Base>& arg = *op.arguments()[0].operation();

            const std::string& argName = createVariableName(arg);

            out() << "(" << argName << " " << _C_COMP_OP_GT << " ";
            printParameter(Base(0.0));
            out() << "?";
            printParameter(Base(1.0));
            out() << ":(" << argName << " " << _C_COMP_OP_LT << " ";
            printParameter(Base(0.0));
            out() << "?";
            printParameter(Base(-1.0));
            out() << ":";
            printParameter(Base(0.0));
            out() << "))";
        }

        virtual void printOperationAdd(SourceCodeFragment<Base>& op) {
            assert(op.arguments().size() == 2);

            print(op.arguments()[0]);
            out() << " + ";
            print(op.arguments()[1]);
        }

        virtual void printOperationMinus(SourceCodeFragment<Base>& op) {
            assert(op.arguments().size() == 2);

            const Argument<Base>& left = op.arguments()[0];
            const Argument<Base>& right = op.arguments()[1];

            const SourceCodeFragment<Base>* opRight = right.operation();
            bool encloseRight = opRight != NULL &&
                    opRight->variableID() == 0 &&
                    opRight->operation() != CGDivOp &&
                    opRight->operation() != CGMulOp &&
                    !isFunction(opRight->operation());

            print(left);
            out() << " - ";
            if (encloseRight) {
                out() << "(";
            }
            print(right);
            if (encloseRight) {
                out() << ")";
            }
        }

        virtual void printOperationDiv(SourceCodeFragment<Base>& op) {
            assert(op.arguments().size() == 2);

            const Argument<Base>& left = op.arguments()[0];
            const Argument<Base>& right = op.arguments()[1];

            const SourceCodeFragment<Base>* opLeft = left.operation();
            bool encloseLeft = opLeft != NULL &&
                    opLeft->variableID() == 0 &&
                    !isFunction(opLeft->operation());
            const SourceCodeFragment<Base>* opRight = right.operation();
            bool encloseRight = opRight != NULL &&
                    opRight->variableID() == 0 &&
                    !isFunction(opRight->operation());

            if (encloseLeft) {
                out() << "(";
            }
            print(left);
            if (encloseLeft) {
                out() << ")";
            }
            out() << " / ";
            if (encloseRight) {
                out() << "(";
            }
            print(right);
            if (encloseRight) {
                out() << ")";
            }
        }

        virtual void printOperationMul(SourceCodeFragment<Base>& op) {
            assert(op.arguments().size() == 2);

            const Argument<Base>& left = op.arguments()[0];
            const Argument<Base>& right = op.arguments()[1];

            const SourceCodeFragment<Base>* opLeft = left.operation();
            bool encloseLeft = opLeft != NULL &&
                    opLeft->variableID() == 0 &&
                    opLeft->operation() != CGDivOp &&
                    opLeft->operation() != CGMulOp &&
                    !isFunction(opLeft->operation());
            const SourceCodeFragment<Base>* opRight = right.operation();
            bool encloseRight = opRight != NULL &&
                    opRight->variableID() == 0 &&
                    opRight->operation() != CGDivOp &&
                    opRight->operation() != CGMulOp &&
                    !isFunction(opRight->operation());

            if (encloseLeft) {
                out() << "(";
            }
            print(left);
            if (encloseLeft) {
                out() << ")";
            }
            out() << " * ";
            if (encloseRight) {
                out() << "(";
            }
            print(right);
            if (encloseRight) {
                out() << ")";
            }
        }

        virtual void printOperationUnaryMinus(SourceCodeFragment<Base>& op) {
            assert(op.arguments().size() == 1);

            const Argument<Base>& arg = op.arguments()[0];

            const SourceCodeFragment<Base>* scf = arg.operation();
            bool enclose = scf != NULL &&
                    scf->variableID() == 0 &&
                    scf->operation() != CGDivOp &&
                    scf->operation() != CGMulOp &&
                    !isFunction(scf->operation());

            out() << "-";
            if (enclose) {
                out() << "(";
            }
            print(arg);
            if (enclose) {
                out() << ")";
            }
        }

        virtual void printConditionalAssignment(SourceCodeFragment<Base>& op) {
            assert(op.variableID() > 0);

            const std::vector<Argument<Base> >& args = op.arguments();
            const Argument<Base> &left = args[0];
            const Argument<Base> &right = args[1];
            const Argument<Base> &trueCase = args[2];
            const Argument<Base> &falseCase = args[3];

            bool isDep = isDependent(op);
            const std::string& varName = createVariableName(op);

            out() << _spaces << "if( ";
            print(left);
            out() << " " << getComparison(op.operation()) << " ";
            print(right);
            out() << " ) {\n";
            out() << _spaces << _spaces << varName << " ";
            out() << (isDep ? _depAssignOperation : "=") << " ";
            print(trueCase);
            out() << ";\n";
            out() << _spaces << "} else {\n";
            out() << _spaces << _spaces << varName << " ";
            out() << (isDep ? _depAssignOperation : "=") << " ";
            print(falseCase);
            out() << ";\n";
            out() << _spaces << "}\n";
        }

        inline bool isDependent(const SourceCodeFragment<Base>& arg) const {
            size_t id = arg.variableID();
            return id > _independentSize && id < _minTemporaryVarID;
        }

        virtual void printParameter(const Base& value) {
            // make sure all digits of floating point values are printed
            out().unsetf(std::ios::floatfield);
            //std::scientific 
            out() << std::setprecision(std::numeric_limits< Base >::digits10 + 2) << value;
        }

        virtual const std::string& getComparison(enum CGOpCode op) const {
            switch (op) {
                case CGComOpLt:
                    return _C_COMP_OP_LT;

                case CGComOpLe:
                    return _C_COMP_OP_LE;

                case CGComOpEq:
                    return _C_COMP_OP_EQ;

                case CGComOpGe:
                    return _C_COMP_OP_GE;

                case CGComOpGt:
                    return _C_COMP_OP_GT;

                case CGComOpNe:
                    return _C_COMP_OP_NE;

                default:
                    CPPAD_ASSERT_UNKNOWN(0);
            }
        }

        static bool isFunction(enum CGOpCode op) {
            return isUnaryFunction(op) || op == CGPowOp;
        }

        static bool isUnaryFunction(enum CGOpCode op) {
            switch (op) {
                case CGAbsOp:
                case CGAcosOp:
                case CGAsinOp:
                case CGAtanOp:
                case CGCoshOp:
                case CGCosOp:
                case CGExpOp:
                case CGLogOp:
                case CGSinhOp:
                case CGSinOp:
                case CGSqrtOp:
                case CGTanhOp:
                case CGTanOp:
                    return true;
                default:
                    return false;
            }
        }

        static bool isCondAssign(enum CGOpCode op) {
            switch (op) {
                case CGComOpLt:
                case CGComOpLe:
                case CGComOpEq:
                case CGComOpGe:
                case CGComOpGt:
                case CGComOpNe:
                    return true;
                default:
                    return false;
            }
        }
    };

    template<class Base>
    const std::string CLanguage<Base>::_C_COMP_OP_LT = "<";
    template<class Base>
    const std::string CLanguage<Base>::_C_COMP_OP_LE = "<=";
    template<class Base>
    const std::string CLanguage<Base>::_C_COMP_OP_EQ = "==";
    template<class Base>
    const std::string CLanguage<Base>::_C_COMP_OP_GE = ">=";
    template<class Base>
    const std::string CLanguage<Base>::_C_COMP_OP_GT = ">";
    template<class Base>
    const std::string CLanguage<Base>::_C_COMP_OP_NE = "!=";
}

#endif