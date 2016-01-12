#ifndef CPPAD_CG_EXPRESSION_NODE_INCLUDED
#define CPPAD_CG_EXPRESSION_NODE_INCLUDED
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
namespace cg {

/**
 * An operation node.
 * 
 * @author Joao Leal
 */
template<class Base>
class OperationNode {
    friend class CodeHandler<Base>;
public:
    typedef typename std::vector<Argument<Base> >::iterator iterator;
    typedef typename std::vector<Argument<Base> >::const_iterator const_iterator;
    typedef typename std::vector<Argument<Base> >::const_reverse_iterator const_reverse_iterator;
    typedef typename std::vector<Argument<Base> >::reverse_iterator reverse_iterator;
public:
    static const std::set<CGOpCode> CUSTOM_NODE_CLASS;
private:
    // the source code handler that own this node (only null for temporary OperationNodes)
    CodeHandler<Base>* handler_;
    // the operations used to create this variable (temporary variables only)
    CGOpCode operation_;
    // additional operation information
    std::vector<size_t> info_;
    // the code blocks this block depends upon (empty for independent 
    // variables and possibly for the 1st assignment of a dependent variable)
    std::vector<Argument<Base> > arguments_;
    // index in the CodeHandler managed nodes array
    size_t pos_;
    // variable ID that was altered/assigned in this source code
    // (zero means that no left-hand variable is assigned)
    size_t var_id_;
    //
    size_t color_;
    // generated variable name
    std::string* name_;
public:
    inline void makeAlias(const Argument<Base>& other) {
        CPPADCG_ASSERT_UNKNOWN(CUSTOM_NODE_CLASS.find(operation_) == CUSTOM_NODE_CLASS.end()); // TODO: consider relaxing this check

        operation_ = CGOpCode::Alias;
        arguments_.resize(1);
        arguments_[0] = other;
        var_id_ = 0;
        delete name_;
        name_ = nullptr;
    }
    
    /**
     * Provides the source code handler that own this node which can be only 
     * null for temporary OperationNodes.
     * 
     * @return a CodeHandler which owns this nodes memory (possibly null)
     */
    inline CodeHandler<Base>* getCodeHandler() const {
        return handler_;
    }

    /**
     * @return Mathematical operation type which this node is the result of.
     */
    inline CGOpCode getOperationType() const {
        return operation_;
    }

    inline void setOperation(CGOpCode op,
                             const std::vector<Argument<Base> >& arguments = std::vector<Argument<Base> >()) {
        CPPADCG_ASSERT_UNKNOWN(op == operation_ || CUSTOM_NODE_CLASS.find(op) == CUSTOM_NODE_CLASS.end()); // cannot transform into a node with a custom class

        operation_ = op;
        arguments_ = arguments;
    }

    /**
     * Provides the arguments used in the operation represented by this
     * code fragment.
     * @return the arguments for the operation in this code fragment
     */
    inline const std::vector<Argument<Base> >& getArguments() const {
        return arguments_;
    }

    inline std::vector<Argument<Base> >& getArguments() {
        return arguments_;
    }

    /**
     * Provides additional information used in the operation.
     * @return the additional operation information/options
     */
    inline const std::vector<size_t>& getInfo() const {
        return info_;
    }

    inline std::vector<size_t>& getInfo() {
        return info_;
    }

    /**
     * Provides the variable ID that was altered/assigned in this source 
     * code (zero means that no variable is assigned).
     * @return the variable ID
     */
    inline size_t getVariableID() const {
        return var_id_;
    }

    /**
     * Specifies a variable ID to the result of this source code
     * (zero means that no variable is created).
     */
    inline void setVariableID(size_t var_id) {
        var_id_ = var_id;
    }

    inline void resetHandlerCounters() {
        var_id_ = 0;
    }

    inline const std::string* getName() const {
        return name_;
    }

    inline void setName(const std::string& name) {
        if (name_ != nullptr)
            *name_ = name;
        else
            name_ = new std::string(name);
    }

    inline void clearName() {
        delete name_;
        name_ = nullptr;
    }

    inline size_t getColor() const {
        return color_;
    }

    inline void setColor(size_t color) {
        color_ = color;
    }
    
    /**
     * Provides the index in CodeHandler which owns this OperationNode.
     * A value of std::numeric_limits<size_t>::max() means that it is not 
     * managed by any CodeHandler.
     * This value can change if its position changes in the CodeHandler.
     * 
     * @return the index in the CodeHandler's array of managed nodes 
     */
    inline size_t getHandlerPosition() const {
        return pos_;
    }
   
    // argument iterators

    inline iterator begin() {
        return arguments_.begin();
    }

    inline const_iterator begin() const {
        return arguments_.begin();
    }

    inline iterator end() {
        return arguments_.end();
    }

    inline const_iterator end() const {
        return arguments_.end();
    }

    inline reverse_iterator rbegin() {
        return arguments_.rbegin();
    }

    inline const_reverse_iterator rbegin() const {
        return arguments_.rbegin();
    }

    inline reverse_iterator rend() {
        return arguments_.rend();
    }

    inline const_reverse_iterator rend() const {
        return arguments_.rend();
    }

    inline const_iterator cbegin() const noexcept {
        return arguments_.cbegin();
    }

    inline const_iterator cend() const noexcept {
        return arguments_.cend();
    }

    inline const_reverse_iterator crbegin() const noexcept {
        return arguments_.crbegin();
    }

    inline const_reverse_iterator crend() const noexcept {
        return arguments_.crend();
    }
    
    inline virtual ~OperationNode() {
        delete name_;
    }
    
protected:
    
    inline OperationNode(const OperationNode& orig) :
        handler_(orig.handler_),
        operation_(orig.operation_),
        info_(orig.info_),
        arguments_(orig.arguments_),
        pos_(std::numeric_limits<size_t>::max()),
        var_id_(0),
        color_(orig.color_),
        name_(orig.name_ != nullptr ? new std::string(*orig.name_) : nullptr) {
    }

    inline OperationNode(CodeHandler<Base>* handler,
                         CGOpCode op) :
        handler_(handler),
        operation_(op),
        pos_(std::numeric_limits<size_t>::max()),
        var_id_(0),
        color_(0),
        name_(nullptr) {
    }

    inline OperationNode(CodeHandler<Base>* handler,
                         CGOpCode op,
                         const Argument<Base>& arg) :
        handler_(handler),
        operation_(op),
        arguments_ {arg},
        pos_(std::numeric_limits<size_t>::max()),
        var_id_(0),
        color_(0),
        name_(nullptr) {
    }

    inline OperationNode(CodeHandler<Base>* handler,
                         CGOpCode op,
                         std::vector<Argument<Base> >&& args) :
        handler_(handler),
        operation_(op),
        arguments_(std::move(args)),
        pos_(std::numeric_limits<size_t>::max()),
        var_id_(0),
        color_(0),
        name_(nullptr) {
    }

    inline OperationNode(CodeHandler<Base>* handler,
                         CGOpCode op,
                         std::vector<size_t>&& info,
                         std::vector<Argument<Base> >&& args) :
        handler_(handler),
        operation_(op),
        info_(std::move(info)),
        arguments_(std::move(args)),
        pos_(std::numeric_limits<size_t>::max()),
        var_id_(0),
        color_(0),
        name_(nullptr) {
    }

    inline OperationNode(CodeHandler<Base>* handler,
                         CGOpCode op,
                         const std::vector<size_t>& info,
                         const std::vector<Argument<Base> >& args) :
        handler_(handler),
        operation_(op),
        info_(info),
        arguments_(args),
        pos_(std::numeric_limits<size_t>::max()),
        var_id_(0),
        color_(0),
        name_(nullptr) {
    }
    
    inline void setHandlerPosition(size_t pos) {
        pos_ = pos;
    }

public:

    /**
     * Creates a temporary operation node.
     * 
     * @warning This node should never be provided to a CodeHandler.
     */
    static std::unique_ptr<OperationNode<Base>> makeTemporaryNode(CGOpCode op,
                                                                  const std::vector<size_t>& info,
                                                                  const std::vector<Argument<Base> >& args) {
        return std::unique_ptr<OperationNode<Base>> (new OperationNode<Base>(nullptr, op, info, args));
    }
    
protected:
    static inline std::set<CGOpCode> makeCustomNodeClassesSet();

};

template<class Base>
inline std::set<CGOpCode> OperationNode<Base>::makeCustomNodeClassesSet() {
    std::set<CGOpCode> s;
    s.insert(CGOpCode::IndexAssign);
    s.insert(CGOpCode::Index);
    s.insert(CGOpCode::LoopStart);
    s.insert(CGOpCode::LoopEnd);
    s.insert(CGOpCode::Pri);
    return s;
}

template<class Base>
const std::set<CGOpCode> OperationNode<Base>::CUSTOM_NODE_CLASS = makeCustomNodeClassesSet();

template<class Base>
inline std::ostream& operator<<(
        std::ostream& os, //< stream to write to
        const OperationNode<Base>& c) {
    CGOpCode op = c.getOperationType();
    switch (op) {
        case CGOpCode::ArrayCreation:
            os << "new $1[" << c.getArguments().size() << "]";
            break;
        case CGOpCode::SparseArrayCreation:
            os << "new $1[" << c.getInfo()[0] << "]";
            break;
        case CGOpCode::ArrayElement:
            os << "$1[" << c.getInfo()[0] << "]";
            break;
        case CGOpCode::AtomicForward:
            os << "atomicFunction.forward(" << c.getInfo()[0] << ", " << c.getInfo()[1] << ", vx, vy, $1, $2)";
            break;
        case CGOpCode::AtomicReverse:
            os << "atomicFunction.reverse(" << c.getInfo()[0] << ", $1, $2, $3, $4)";
            break;
        case CGOpCode::Sign:
            os << "if($1 > 0) { 1 } else if($1 == 0) { 0 } else { -1 }";
            break;

        default:
            os << op;
    }

    return os;
}

} // END cg namespace
} // END CppAD namespace

#endif