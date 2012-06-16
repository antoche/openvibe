#ifndef __OpenViBEPlugins_SignalProcessing_CEquationParser_H__
#define __OpenViBEPlugins_SignalProcessing_CEquationParser_H__

#include "../ovp_defines.h"

#include "ovpCEquationParserGrammar.h"
#include "ovpCAbstractTree.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_ast.hpp>

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>

using namespace boost::spirit;

typedef char const* iterator_t;
typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t;

class CAbstractTree;
class CAbstractTreeNode;

/**
* Used to store the optional parameter of the function used by the pseudo-VM.
*
*/
union functionContext
{
	OpenViBE::float64 m_f64DirectValue;//if the parameter if a value (push_val)
	OpenViBE::float64** m_ppIndirectValue;//if it is a pointer to a value (push_var)
} ;

//! Type of the functions in the function stack generated from the equation.
typedef void (*functionPointer)(OpenViBE::float64 *& pStack, functionContext& oContext);

class CEquationParser
{
protected:

	//! The AST produced by the parsing of the equation
	CAbstractTree * m_pTree;

	//! Grammar to use
	CEquationGrammar m_oGrammar;

	//! Pointer to the data referenced by X in the equation
	OpenViBE::float64** m_ppVariable;
	//! Number of accessible variables
	OpenViBE::uint32 m_ui32VariableCount;

	//! Size of the "function stack" (where the sucessive function pointers are stored)
	const OpenViBE::uint32 m_ui32FunctionStackSize;
	//! Pointer to the top of the function stack
	functionPointer * m_pFunctionList;
	//! Pointer to the base of the function stack
	functionPointer * m_pFunctionListBase;

	//! Size of the "function context stack" (where the sucessive function context are stored)
	const OpenViBE::uint64 m_ui64FunctionContextStackSize;
	//! Pointer to the top of the function context stack
	functionContext * m_pFunctionContextList;
	//! Pointer to the base of the function context stack
	functionContext * m_pFunctionContextListBase;

	//! Size of the "local" stack
	const OpenViBE::uint64 m_ui64StackSize;
	//! Pointer to the top of the "local" stack
	OpenViBE::float64 * m_pStack;

	//! Number of pointers/contexts in the function/context stacks (same for both)
	OpenViBE::uint64 m_ui64NumberOfOperations;

	//! Table of function pointers
	static functionPointer m_pFunctionTable[32];

	//! Category of the tree (OP_USERDEF or Special tree)
	OpenViBE::uint64 m_ui64TreeCategory;
	//! Optional parameter in case of a special tree
	OpenViBE::float64 m_f64TreeParameter;

	OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& m_oParentPlugin;

public:

	/**
	* Constructor.
	* \param pVariable Pointer to the data known as X in the equation.
	*/
	CEquationParser(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin, OpenViBE::float64** ppVarialbe, OpenViBE::uint32 ui32VariableCount);

	//! Destructor.
	~CEquationParser();

#if 0
	//
	void setVariable(OpenViBE::float64 * pVariable){ m_pVariable=pVariable; }
#endif

	/**
	* Compiles the given equation, and generates the successive function calls to achieve the
	* same result if needed (depends on m_ui64TreeCategory).
	* \param pEquation The equation to use.
	*/
	OpenViBE::boolean compileEquation(const char * pEquation);

	void push_op(OpenViBE::uint64 ui64Operator);
	void push_value(OpenViBE::float64 f64Value);
	void push_var(OpenViBE::uint32 ui32Index);

	/**
	* Returns the tree's category.
	* \return The tree's category.
	*/
	OpenViBE::uint64 getTreeCategory() const { return m_ui64TreeCategory; }

	/**
	 * Returns the optional parameter.
	 * \return The optional parameter.
	 */
	OpenViBE::float64 getTreeParameter() const { return m_f64TreeParameter; }

	/**
	* Executes the successive function calls from the function stack and returns
	* the result.
	* \return The result of the equation applied to the value referenced by X.
	*/
	OpenViBE::float64 executeEquation()
	{
		functionPointer * l_pCurrentFunction = m_pFunctionList - 1;
		functionPointer * l_pLastFunctionPointer = l_pCurrentFunction - m_ui64NumberOfOperations;

		functionContext * l_pCurrentFunctionContext = m_pFunctionContextList - 1;

		//while there are function pointers
		while(l_pCurrentFunction != l_pLastFunctionPointer)
		{
			//calls the function with the current function context
			(*l_pCurrentFunction)(m_pStack, *l_pCurrentFunctionContext);

			//updates the stack pointers
			l_pCurrentFunction--;
			l_pCurrentFunctionContext--;
		}

		//pop and return the result
		return *(m_pStack--);
	}

private:

	void createAbstractTree(tree_parse_info<> oInfo);
	CAbstractTreeNode * createNode(iter_t const& i);

public:

	static void op_neg(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_add(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_div(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_sub(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_mul(OpenViBE::float64*& pStack, functionContext& pContext);

	static void op_power(OpenViBE::float64*& pStack, functionContext& pContext);

	static void op_abs(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_acos(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_asin(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_atan(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_ceil(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_cos(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_exp(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_floor(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_log(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_log10(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_sin(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_sqrt(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_tan(OpenViBE::float64*& pStack, functionContext& pContext);

	static void op_if_then_else(OpenViBE::float64*& pStack, functionContext& pContext);

	static void op_cmp_lower(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_cmp_greater(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_cmp_lower_equal(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_cmp_greater_equal(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_cmp_equal(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_cmp_not_equal(OpenViBE::float64*& pStack, functionContext& pContext);

	static void op_bool_and(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_bool_or(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_bool_not(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_bool_xor(OpenViBE::float64*& pStack, functionContext& pContext);

	static void op_loadVal(OpenViBE::float64*& pStack, functionContext& pContext);
	static void op_loadVar(OpenViBE::float64*& pStack, functionContext& pContext);
};




#endif
