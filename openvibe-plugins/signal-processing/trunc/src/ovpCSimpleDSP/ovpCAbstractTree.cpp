#include "ovpCAbstractTree.h"

using namespace std;
using namespace boost::spirit;
using namespace OpenViBE;

//#define CABSTRACTTREE_DEBUG

void CAbstractTree::simplifyTree()
{
	boolean l_bHasChanged = true;

	//while stability hasn't been reached
	while(l_bHasChanged)
	{
		CAbstractTreeNode* l_pSimplifiedNode = m_pRoot;

		//tries to simplify the tree.
		l_bHasChanged = m_pRoot->simplify(l_pSimplifiedNode);

		//if the root node has changed
		if(l_pSimplifiedNode != m_pRoot)
		{
			//delete the old one
			delete m_pRoot;
			m_pRoot = l_pSimplifiedNode;
		}
	}
}

void CAbstractTree::levelOperators()
{
	m_pRoot->levelOperators();
}

// Dirty hack to avoid GCC 4.3 crash at compilation time
static void clear_m_oChildren(std::vector<CAbstractTreeNode *>& m_oChildren)
{
	for(size_t i=0 ; i<m_oChildren.size() ; i++)
	{
		delete m_oChildren[i];
	}
}

CAbstractTreeParentNode::~CAbstractTreeParentNode(void)
{
	// Dirty hack to avoid GCC 4.3 crash at compilation time
	::clear_m_oChildren(m_oChildren);
}

void CAbstractTreeParentNode::levelOperators()
{
	uint64 l_ui64NumberOfChildren = m_oChildren.size();

	vector<CAbstractTreeNode*> l_oNewChildren;

	//for all the node's children
	for(uint64 i=0 ; i<l_ui64NumberOfChildren ; i++)
	{
		CAbstractTreeNode * l_pChild = m_oChildren[(size_t)i];

		//recursively try to level the childs' operators
		l_pChild->levelOperators();

		//if the child is a terminal node
		if(l_pChild->isTerminal())
		{
			//add it to the children list
			l_oNewChildren.push_back(l_pChild);
		}
		else
		{
			//else it's a parent node
			CAbstractTreeParentNode * l_pChildParentNode = reinterpret_cast<CAbstractTreeParentNode*>(l_pChild);

			//if the child and the current node have the same id
			if(m_ui64Identifier == l_pChildParentNode->getOperatorIdentifier())
			{
				switch(m_ui64Identifier)
				{
					//check if it is the ID of the + or * operators
					case OP_ADD:
					case OP_MUL:

						//if it is, we can group the child's children with the current node's children
						l_oNewChildren.insert( l_oNewChildren.end(), l_pChildParentNode->getChildren().begin(), l_pChildParentNode->getChildren().end() );

						//we don't want it to destroy its old children
						l_pChildParentNode->getChildren().clear();

						//we no longer need this child
						delete l_pChildParentNode;
						l_pChildParentNode = NULL;

						break;

					default:
						//this kind of node isn't an associative one, so keep the child
						l_oNewChildren.push_back(l_pChild);
						break;
				}
			}
			else
			{
				l_oNewChildren.push_back(l_pChild);
			}
		}
	}

	m_oChildren = l_oNewChildren;

	//for + or *
	if(isAssociative())
	{
		//if the node is associative/commutative, reorder the children
		sort(m_oChildren.begin(), m_oChildren.end(), CAbstractTreeNodeOrderingFunction());
	}
}

boolean CAbstractTreeParentNode::simplify(CAbstractTreeNode *& pModifiedNode)
{
	//result boolean, true if a child has changed
	boolean l_bHasChanged = false;

	//true if a child has changed
	boolean l_bChildrenChanged = true;

	//number of children of this node
	uint64 l_ui64NumberOfChildren = m_oChildren.size();

	//while the children aren't stable
	while(l_bChildrenChanged)
	{
		l_bChildrenChanged = false;

		//try to simplify all the children
		for(size_t i=0 ; i<l_ui64NumberOfChildren ; i++)
		{
			CAbstractTreeNode * l_pChild = m_oChildren[i];
			l_bChildrenChanged = l_pChild->simplify(l_pChild);

			//if there has been a change, actualize l_bHasChanged
			l_bHasChanged |= l_bChildrenChanged;

			//if the child has become a new node
			if(m_oChildren[i] != l_pChild)
			{
				//delete the old one and replace it
				delete m_oChildren[i];
				m_oChildren[i] = l_pChild;
			}
		}
	}

	//unary operator
	if(l_ui64NumberOfChildren == 1)
	{
		//if we can already compute the result
		if(m_oChildren[0]->isConstant())
		{
			float64 l_f64ChildValue = reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue();
			switch(m_ui64Identifier)
			{
				case OP_NEG: pModifiedNode = new CAbstractTreeValueNode(-l_f64ChildValue); break;
				case OP_ABS: pModifiedNode = new CAbstractTreeValueNode(abs(l_f64ChildValue)); break;
				case OP_ACOS: pModifiedNode = new CAbstractTreeValueNode(acos(l_f64ChildValue)); break;
				case OP_ASIN: pModifiedNode = new CAbstractTreeValueNode(asin(l_f64ChildValue)); break;
				case OP_ATAN: pModifiedNode = new CAbstractTreeValueNode(atan(l_f64ChildValue)); break;
				case OP_CEIL: pModifiedNode = new CAbstractTreeValueNode(ceil(l_f64ChildValue)); break;
				case OP_COS: pModifiedNode = new CAbstractTreeValueNode(cos(l_f64ChildValue)); break;
				case OP_EXP: pModifiedNode = new CAbstractTreeValueNode(exp(l_f64ChildValue)); break;
				case OP_FLOOR: pModifiedNode = new CAbstractTreeValueNode(floor(l_f64ChildValue)); break;
				case OP_LOG: pModifiedNode = new CAbstractTreeValueNode(log(l_f64ChildValue)); break;
				case OP_LOG10: pModifiedNode = new CAbstractTreeValueNode(log10(l_f64ChildValue)); break;
				case OP_SIN: pModifiedNode = new CAbstractTreeValueNode(sin(l_f64ChildValue)); break;
				case OP_SQRT: pModifiedNode = new CAbstractTreeValueNode(sqrt(l_f64ChildValue)); break;
				case OP_TAN: pModifiedNode = new CAbstractTreeValueNode(tan(l_f64ChildValue)); break;
			}
			l_bHasChanged = true;
		}
	}
	//binary operator not associative
	else if(l_ui64NumberOfChildren == 2 && !isAssociative())
	{
		float64 l_f64TotalValue=0;

		//if we can already compute the result
		if(m_oChildren[0]->isConstant() && m_oChildren[1]->isConstant())
		{
			switch(m_ui64Identifier)
			{
				case OP_DIV:
					l_f64TotalValue = reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue() / reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue();

					//delete the old value nodes
					delete m_oChildren[0];
					m_oChildren[0] = NULL;
					delete m_oChildren[1];
					m_oChildren[1] = NULL;

					pModifiedNode = new CAbstractTreeValueNode(l_f64TotalValue);
					l_bHasChanged = true;

					break;

				case OP_POW:

					l_f64TotalValue = pow(reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue() , reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue() );

					//delete the old value nodes
					delete m_oChildren[0];
					m_oChildren[0] = NULL;
					delete m_oChildren[1];
					m_oChildren[1] = NULL;

					pModifiedNode = new CAbstractTreeValueNode(l_f64TotalValue);
					l_bHasChanged = true;
					break;
			}
		}

		//test special cases (X/1), ..., simplify
		else if(m_ui64Identifier == OP_DIV)
		{
			if(!m_oChildren[0]->isConstant() && m_oChildren[1]->isConstant())
			{
				if( reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue() == 1 )
				{
					pModifiedNode = m_oChildren[0];
					m_oChildren.clear();
					l_bHasChanged = true;
				}
			}
		}
	}
	//if the node is an associative operation node, there are at least two children and at least two are constants
	else if(l_ui64NumberOfChildren >= 2 && isAssociative())
	{
		//For commutative nodes
		//The order of the children may have changed due to previous child simplification
		sort(m_oChildren.begin(), m_oChildren.end(), CAbstractTreeNodeOrderingFunction());

		//the new children if there are changes
		vector<CAbstractTreeNode *> l_oNewChildren;

		//iterator on the children
		size_t i=0;
		float64 l_f64TotalValue=0;

		switch(m_ui64Identifier)
		{
			case OP_ADD:
				l_f64TotalValue = 0;

				//add the values of all the constant children
				for(i=0 ; i<l_ui64NumberOfChildren && m_oChildren[i]->isConstant(); i++)
				{
					l_f64TotalValue += reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[i])->getValue();

					//delete the old value nodes
					delete m_oChildren[i];
					m_oChildren[i] = NULL;
				}
				break;

			case OP_MUL:
				l_f64TotalValue = 1;
				//multiply the values of all the constant children
				for(i=0 ; i<l_ui64NumberOfChildren && m_oChildren[i]->isConstant(); i++)
				{
					l_f64TotalValue *= reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[i])->getValue();

					//delete the old value nodes
					delete m_oChildren[i];
					m_oChildren[i] = NULL;
				}
				break;
		}

		//if there were only value nodes, we can replace the current parent node by a value node
		if(i == l_ui64NumberOfChildren)
		{
			pModifiedNode = new CAbstractTreeValueNode(l_f64TotalValue);
			l_bHasChanged = true;
			// cout<<l_f64TotalValue<<endl;
		}
		//if there are still some other children, but we reduced at least two children
		else if(i > 1)
		{
			//adds the new result node to the list
			l_oNewChildren.push_back(new CAbstractTreeValueNode(l_f64TotalValue));

			//adds the other remaining children
			for(; i<l_ui64NumberOfChildren; i++)
			{
				l_oNewChildren.push_back(m_oChildren[i]);
			}
			//we keep this node, but modify its children
			m_oChildren = l_oNewChildren;

			l_bHasChanged = true;
		}
		else if(i == 1)
		{
			//nothing changed
			if( (l_f64TotalValue == 0 && m_ui64Identifier==OP_ADD) ||
			(l_f64TotalValue == 1 && m_ui64Identifier==OP_MUL) )
			{
				if(l_ui64NumberOfChildren-i == 1)
				{
					pModifiedNode = m_oChildren[i];
					m_oChildren.clear();
				}
				else
				{
					//don't keep the valueNode
					//adds the other remaining children
					for(; i<l_ui64NumberOfChildren; i++)
					{
						l_oNewChildren.push_back(m_oChildren[i]);
					}

					//we keep this node, but modify its children
					m_oChildren = l_oNewChildren;
				}
				l_bHasChanged = true;
			}
			else if(l_f64TotalValue == 0 && m_ui64Identifier==OP_MUL)
			{
				//kill this node and replace it by a 0 node
				pModifiedNode = new CAbstractTreeValueNode(0);
				l_bHasChanged = true;
			}
			else
			{
				//undo changes
				m_oChildren[0] = new CAbstractTreeValueNode(l_f64TotalValue);
			}
		}
	}

	return l_bHasChanged;
}

void CAbstractTreeParentNode::useNegationOperator()
{
	uint64 l_ui64NumberOfChildren = m_oChildren.size();

	//try to use the negation operator in all the children
	for(size_t i=0 ; i<l_ui64NumberOfChildren ; i++)
	{
		CAbstractTreeNode * l_pChild = m_oChildren[i];
		l_pChild->useNegationOperator();
	}

	//replace (/ Something -1) by (NEG Something)
	if(m_ui64Identifier == OP_DIV)
	{
		if(m_oChildren[1]->isConstant())
		{
			if(reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue() == -1)
			{
				m_ui64Identifier = OP_NEG;
				m_oChildren.pop_back();
			}
		}
	}
	//replace (* -1 ...) by (NEG (* ...))
	else if(m_ui64Identifier == OP_MUL)
	{
		if(m_oChildren[0]->isConstant())
		{
			if(reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue() == -1)
			{
				m_ui64Identifier = OP_NEG;
				m_bIsAssociative = false;

				//if there were just two children : replace (* -1 Sth) by (NEG Sth)
				if(l_ui64NumberOfChildren==2)
				{
					m_oChildren[0] = m_oChildren[1];
					m_oChildren.pop_back();
				}
				//>2 there were more than two children
				else
				{
					CAbstractTreeParentNode * l_pNewOperatorNode = new CAbstractTreeParentNode(OP_MUL, true);

					for(size_t i=1 ; i<l_ui64NumberOfChildren ; i++)
					{
						l_pNewOperatorNode->addChild(m_oChildren[i]);
					}

					m_oChildren.clear();
					m_oChildren.push_back(l_pNewOperatorNode);
				}
			}
		}
	}
}

void CAbstractTree::generateCode(CEquationParser& oParser)
{
	m_pRoot->generateCode(oParser);
}

void CAbstractTreeParentNode::generateCode(CEquationParser& oParser)
{
	uint64 l_ui64NumberOfChildren = m_oChildren.size();

#if 0

// REMOVED BY YRD
// Bruno's implementation of the stack filling looked weird
// and bugged to me. I simplified/corrected it

	//if it is a unary operator/function
	if(l_ui64NumberOfChildren==1)
	{
		oParser.push_op(m_ui64Identifier);
	}
	else
	{
		for(size_t i = 0 ; i<l_ui64NumberOfChildren-1 ; i++)
		{
			oParser.push_op(m_ui64Identifier);
			m_oChildren[i] -> generateCode(oParser);
		}
	}
	m_oChildren[(size_t)(l_ui64NumberOfChildren-1)] -> generateCode(oParser);
#else
	oParser.push_op(m_ui64Identifier);
	for(size_t i=0; i<l_ui64NumberOfChildren; i++)
	{
		m_oChildren[i] -> generateCode(oParser);
	}
#endif
}

void CAbstractTreeValueNode::generateCode(CEquationParser& oParser)
{
	oParser.push_value(m_f64Value);
}

void CAbstractTreeVariableNode::generateCode(CEquationParser& oParser)
{
	oParser.push_var(m_ui32Index);
}

void CAbstractTree::recognizeSpecialTree(uint64& ui64TreeIdentifier, float64& f64Parameter)
{
	//default
	ui64TreeIdentifier = OP_USERDEF;
	f64Parameter = 0;

	//the root node is a value node or variable node
	if(m_pRoot->isTerminal())
	{
		//if it is a variable node
		if(!m_pRoot->isConstant())
		{
			ui64TreeIdentifier = OP_NONE;
		}
		return;
	}

	CAbstractTreeParentNode * l_pParent = reinterpret_cast<CAbstractTreeParentNode *>(m_pRoot);

	std::vector<CAbstractTreeNode *>& l_oChildren = l_pParent->getChildren();
	uint64 l_ui64NumberOfChildren = l_oChildren.size();
	uint64 l_ui64NodeIdentifier = l_pParent->getOperatorIdentifier();

	//unary operator/function
	if(l_ui64NumberOfChildren == 1)
	{
		if( l_oChildren[0]->isTerminal() && !l_oChildren[0]->isConstant())
		{
			ui64TreeIdentifier = l_ui64NodeIdentifier;
		}
	}
	//binary
	else if(l_ui64NumberOfChildren == 2)
	{
		boolean l_bIsVariable[2];
		l_bIsVariable[0] = l_oChildren[0]->isTerminal() & !l_oChildren[0]->isConstant();
		l_bIsVariable[1] = l_oChildren[1]->isTerminal() & !l_oChildren[1]->isConstant();

		//(* X X)
		if(l_ui64NodeIdentifier == OP_MUL && l_bIsVariable[0] && l_bIsVariable[1] )
		{
			ui64TreeIdentifier = OP_X2;
		}
		//pow(X,2)
		else if (l_ui64NodeIdentifier == OP_POW && l_bIsVariable[0] && l_oChildren[1]->isConstant() )
		{
			ui64TreeIdentifier = OP_X2;
		}
		//(+ Cst X) or (* Cst X)
		else if ( l_pParent->isAssociative() && l_oChildren[0]->isConstant() && l_bIsVariable[1])
		{
			ui64TreeIdentifier = l_ui64NodeIdentifier;
			f64Parameter = reinterpret_cast<CAbstractTreeValueNode*>(l_oChildren[0])->getValue();
		}
		// (/ X Cst)
		else if ( l_ui64NodeIdentifier==OP_DIV && l_bIsVariable[0] && l_oChildren[1]->isConstant())
		{
			ui64TreeIdentifier = OP_DIV;
			f64Parameter = reinterpret_cast<CAbstractTreeValueNode*>(l_oChildren[1])->getValue();
		}
	}
	//else do nothing
}
