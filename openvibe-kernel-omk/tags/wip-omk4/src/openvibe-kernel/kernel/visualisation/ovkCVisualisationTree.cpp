#include "../../ovk_base.h"
#include "ovkCVisualisationTree.h"

#include <iostream>
#include <map>
#include <vector>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace std;

namespace
{
	template <class T>
	struct TTestTrue
	{
		boolean operator()(typename map<CIdentifier, T*>::const_iterator it) const { return true; }
	};

	struct TTestEqVisualisationWidgetType
	{
		TTestEqVisualisationWidgetType(EVisualisationWidgetType oType) : m_oType(oType) { }
		boolean operator()(map<CIdentifier, IVisualisationWidget*>::const_iterator it) const
		{
			return it->second->getType() == m_oType;
		}
		EVisualisationWidgetType m_oType;
	};

	struct TTestEqVisualisationWidgetFromBoxIdentifier
	{
		TTestEqVisualisationWidgetFromBoxIdentifier(const CIdentifier& rBoxId) : m_rBoxId(rBoxId) { }
		boolean operator()(map<CIdentifier, IVisualisationWidget*>::const_iterator it) const
		{
			return it->second->getBoxIdentifier() == m_rBoxId;
		}
		const CIdentifier& m_rBoxId;
	};

	template <class T, class TTest>
	boolean getNextTIdentifier(
		const map<CIdentifier, T*>& vMap,
		CIdentifier& rIdentifier,
		const TTest& rTest)
	{
		typename map<CIdentifier, T*>::const_iterator it;

		if(rIdentifier==OV_UndefinedIdentifier)
		{
			it=vMap.begin();
		}
		else
		{
			it=vMap.find(rIdentifier);
			if(it==vMap.end())
			{
				rIdentifier = OV_UndefinedIdentifier;
				return false;
			}
			it++;
		}

		while(it!=vMap.end())
		{
			if(rTest(it))
			{
				rIdentifier = it->first;
				return true;
			}
			it++;
		}

		return false;
	}
};

CVisualisationTree::CVisualisationTree(const OpenViBE::Kernel::IKernelContext& rKernelContext) :
	TKernelObject<TAttributable<IVisualisationTree> >(rKernelContext),
	m_rKernelContext(rKernelContext),
	m_pScenario(NULL),
	m_pTreeStore(NULL),
	m_pTreeViewCB(NULL)
{
}

CVisualisationTree::~CVisualisationTree()
{
	//delete display panels
	//TODO!

	g_object_unref(m_pTreeStore);
}

boolean CVisualisationTree::init(const OpenViBE::Kernel::IScenario* pScenario)
{
	m_pScenario = pScenario;

	//create tree store
	m_pTreeStore = gtk_tree_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_ULONG, G_TYPE_STRING, G_TYPE_POINTER);

	return true;
}

boolean CVisualisationTree::getNextVisualisationWidgetIdentifier(CIdentifier& rIdentifier) const
{
	return getNextTIdentifier<IVisualisationWidget, TTestTrue<IVisualisationWidget> >
		(m_vVisualisationWidget, rIdentifier, TTestTrue<IVisualisationWidget>());
}

boolean CVisualisationTree::getNextVisualisationWidgetIdentifier(CIdentifier& rIdentifier, EVisualisationWidgetType oType) const
{
	return getNextTIdentifier<IVisualisationWidget, TTestEqVisualisationWidgetType >
		(m_vVisualisationWidget, rIdentifier, TTestEqVisualisationWidgetType(oType));
}

boolean	CVisualisationTree::isVisualisationWidget(const CIdentifier& rIdentifier) const
{
	return m_vVisualisationWidget.find(rIdentifier) != m_vVisualisationWidget.end();
}

IVisualisationWidget* CVisualisationTree::getVisualisationWidget(const CIdentifier& rIdentifier) const
{
	map<CIdentifier, IVisualisationWidget*>::const_iterator it = m_vVisualisationWidget.find(rIdentifier);

	if(it == m_vVisualisationWidget.end())
	{
		return NULL;
	}
	return it->second;
}

IVisualisationWidget* CVisualisationTree::getVisualisationWidgetFromBoxIdentifier(const CIdentifier& rBoxIdentifier) const
{
	CIdentifier l_oIdentifier = OV_UndefinedIdentifier;
	getNextTIdentifier<IVisualisationWidget, TTestEqVisualisationWidgetFromBoxIdentifier >
		(m_vVisualisationWidget, l_oIdentifier, TTestEqVisualisationWidgetFromBoxIdentifier(rBoxIdentifier));
	return getVisualisationWidget(l_oIdentifier);
}

boolean CVisualisationTree::addVisualisationWidget(CIdentifier& rIdentifier, const CString& rName, EVisualisationWidgetType oType,
	const CIdentifier& rParentIdentifier, uint32 ui32Index, const CIdentifier& rBoxIdentifier, uint32 ui32NbChildren)
{
	log() << LogLevel_Info << "Adding new visualisation widget\n";

	//create new widget
	IVisualisationWidget* l_pVisualisationWidget = OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IVisualisationWidget*>(OV_ClassId_Kernel_Visualisation_VisualisationWidget);
	rIdentifier=getUnusedIdentifier();

	if(l_pVisualisationWidget->initialize(rIdentifier, rName, oType, rParentIdentifier, rBoxIdentifier, ui32NbChildren) == false)
	{
		log() << LogLevel_Warning << "Failed to add new visualisation widget (couldn't initialize it)\n";
		getKernelContext().getKernelObjectFactory().releaseObject(l_pVisualisationWidget);
		return false;
	}

	//parent it
	if(rParentIdentifier != OV_UndefinedIdentifier)
	{
		IVisualisationWidget* l_pParentVisualisationWidget = getVisualisationWidget(rParentIdentifier);

		if(l_pParentVisualisationWidget != NULL)
		{
			if(l_pParentVisualisationWidget->getType() == EVisualisationWidget_VisualisationWindow)
			{
				//extend number of children of parent window if necessary
				if(l_pParentVisualisationWidget->getNbChildren() <= ui32Index)
				{
					for(unsigned int i = l_pParentVisualisationWidget->getNbChildren(); i<=ui32Index; i++)
					{
						l_pParentVisualisationWidget->addChild(OV_UndefinedIdentifier);
					}
				}
			}

			if(l_pParentVisualisationWidget->setChildIdentifier(ui32Index, rIdentifier) == false)
			{
				log() << LogLevel_Warning << "Failed to add new visualisation widget (couldn't set child identifier in parent window)\n";
				return false;
			}
		}
		else
		{
			log() << LogLevel_Warning << "Failed to add new visualisation widget (couldn't find parent)\n";
			return false;
		}
	}

	//add it to widgets vector
	m_vVisualisationWidget[rIdentifier]=l_pVisualisationWidget;
	return true;
}

boolean CVisualisationTree::getVisualisationWidgetIndex(const CIdentifier& rIdentifier, uint32& rIndex) const
{
	//retrieve widget
	IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(rIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		return false;
	}

	//retrieve parent widget
	const CIdentifier& l_rParentIdentifier = l_pVisualisationWidget->getParentIdentifier();
	if(l_rParentIdentifier == OV_UndefinedIdentifier)
	{
		return false;
	}

	IVisualisationWidget* l_pParentVisualisationWidget = getVisualisationWidget(l_rParentIdentifier);
	if(l_pParentVisualisationWidget == NULL)
	{
		log() << LogLevel_Warning << "Failed to unparent visualisation widget (couldn't find parent)\n";
		return false;
	}

	l_pParentVisualisationWidget->getChildIndex(rIdentifier, rIndex);

	return true;
}

boolean CVisualisationTree::destroyHierarchy(const CIdentifier& rIdentifier, boolean bDestroyVisualisationBoxes)
{
	boolean res = true;

	IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(rIdentifier);

	//is hierarchy top item a window?
	if(l_pVisualisationWidget->getType() == EVisualisationWidget_VisualisationWindow)
	{
		CIdentifier l_oChildIdentifier;
		for(uint32 i=0; i<l_pVisualisationWidget->getNbChildren(); i++)
		{
			l_pVisualisationWidget->getChildIdentifier(i, l_oChildIdentifier);
			res &=_destroyHierarchy(l_oChildIdentifier, bDestroyVisualisationBoxes);
		}

		//delete this window in kernel factory and erase its slot in map
		log() << LogLevel_Trace << "Deleting visualisation window\n";
		getKernelContext().getKernelObjectFactory().releaseObject(l_pVisualisationWidget);
		map<CIdentifier, IVisualisationWidget*>::iterator it = m_vVisualisationWidget.find(rIdentifier);
		m_vVisualisationWidget.erase(it);
	}
	else //top item is a widget
	{
		uint32 l_ui32Index;
		unparentVisualisationWidget(rIdentifier, l_ui32Index);
		_destroyHierarchy(rIdentifier, bDestroyVisualisationBoxes);
	}

	return res;
}

boolean CVisualisationTree::_destroyHierarchy(const CIdentifier& rIdentifier, boolean bDestroyVisualisationBoxes)
{
	IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(rIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		return false;
	}

	//remove children
	CIdentifier l_oChildIdentifier;
	int nbChildren = l_pVisualisationWidget->getNbChildren();
	for(int i=0; i<nbChildren; i++)
	{
		l_pVisualisationWidget->getChildIdentifier(i, l_oChildIdentifier);
		_destroyHierarchy(l_oChildIdentifier, bDestroyVisualisationBoxes);
	}

	//if parent widget is a window, remove this widget from it
	if(l_pVisualisationWidget->getType() == EVisualisationWidget_VisualisationPanel)
	{
		IVisualisationWidget* l_pVisualisationWindow = getVisualisationWidget(l_pVisualisationWidget->getParentIdentifier());
		if(l_pVisualisationWindow != NULL)
		{
			l_pVisualisationWindow->removeChild(rIdentifier);
		}
	}

	//if this widget is a visualisation box and they are to be unaffected
	if(l_pVisualisationWidget->getType() == EVisualisationWidget_VisualisationBox && bDestroyVisualisationBoxes == false)
	{
		uint32 l_ui32Index;
		unparentVisualisationWidget(rIdentifier, l_ui32Index);
	}
	else
	{
		log() << LogLevel_Trace << "Deleting visualisation widget\n";
		getKernelContext().getKernelObjectFactory().releaseObject(l_pVisualisationWidget);
		map<CIdentifier, IVisualisationWidget*>::iterator it = m_vVisualisationWidget.find(rIdentifier);
		m_vVisualisationWidget.erase(it);
	}

	return true;
}

boolean	CVisualisationTree::unparentVisualisationWidget(const CIdentifier& rIdentifier, uint32& ui32Index)
{
	//retrieve widget to be unparented
	IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(rIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		return false;
	}

	//get its parent identifier
	const CIdentifier& l_rParentIdentifier = l_pVisualisationWidget->getParentIdentifier();
	if(l_rParentIdentifier == OV_UndefinedIdentifier)
	{
		return true;
	}

	//unparent widget
	l_pVisualisationWidget->setParentIdentifier(OV_UndefinedIdentifier);

	//retrieve parent and remove widget from its children list
	IVisualisationWidget* l_pParentVisualisationWidget = getVisualisationWidget(l_rParentIdentifier);
	if(l_pParentVisualisationWidget != NULL)
	{
		l_pParentVisualisationWidget->getChildIndex(rIdentifier, ui32Index);
		l_pParentVisualisationWidget->removeChild(rIdentifier);
	}

	return true;
}

boolean CVisualisationTree::parentVisualisationWidget(const CIdentifier& rIdentifier, const CIdentifier& rParentIdentifier, uint32 ui32Index)
{
	if(rParentIdentifier == OV_UndefinedIdentifier)
	{
		return false;
	}

	//retrieve widget to be parented
	IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(rIdentifier);
	if(l_pVisualisationWidget == NULL)
	{
		return false;
	}

	l_pVisualisationWidget->setParentIdentifier(rParentIdentifier);

	//retrieve its parent
	IVisualisationWidget* l_pParentVisualisationWidget = getVisualisationWidget(rParentIdentifier);
	if(l_pParentVisualisationWidget == NULL)
	{
		log() << LogLevel_Warning << "Failed to parent visualisation widget (couldn't find parent)\n";
		return false;
	}

	l_pParentVisualisationWidget->setChildIdentifier(ui32Index, rIdentifier);

	return true;
}

CIdentifier CVisualisationTree::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, IVisualisationWidget*>::const_iterator i;
	do
	{
		l_oResult=CIdentifier(l_ui64Identifier++);
		i=m_vVisualisationWidget.find(l_oResult);
	}
	while(i!= m_vVisualisationWidget.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}

::GtkTreeView* CVisualisationTree::createTreeViewWithModel()
{
	return GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(m_pTreeStore)));
}

boolean CVisualisationTree::setTreeViewCB(ITreeViewCB* pTreeViewCB)
{
	m_pTreeViewCB = pTreeViewCB;
	return true;
}

boolean CVisualisationTree::reloadTree()
{
	//clear current tree
	::GtkTreeIter l_oIter;
	while(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oIter, NULL, 0) != FALSE)
		gtk_tree_store_remove(m_pTreeStore, &l_oIter);

	//create 'unaffected display plugins' node
	gtk_tree_store_append(m_pTreeStore, &l_oIter, NULL);
	gtk_tree_store_set(m_pTreeStore, &l_oIter,
		EVisualisationTreeColumn_StringName, "Unaffected display plugins",
		EVisualisationTreeColumn_StringStockIcon, getTreeWidgetIcon(EVisualisationTreeNode_Unaffected),
		EVisualisationTreeColumn_ULongNodeType, EVisualisationTreeNode_Unaffected,
		EVisualisationTreeColumn_StringIdentifier, (const char*)OV_UndefinedIdentifier.toString(),
		-1);

	//reload unaffected visualisation boxes
	CIdentifier l_oVisualisationWidgetIdentifier = OV_UndefinedIdentifier;
	while(getNextVisualisationWidgetIdentifier(l_oVisualisationWidgetIdentifier, EVisualisationWidget_VisualisationBox) == true)
	{
		IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(l_oVisualisationWidgetIdentifier);
		//load widget if it doesn't have a parent (== is unaffected)
		if(l_pVisualisationWidget->getParentIdentifier() == OV_UndefinedIdentifier)
			loadVisualisationWidget(&l_oIter, 0, l_pVisualisationWidget);
	}

	//reload visualisation windows
	CIdentifier l_oVisualisationWindowIdentifier = OV_UndefinedIdentifier;
	while(getNextVisualisationWidgetIdentifier(l_oVisualisationWindowIdentifier, EVisualisationWidget_VisualisationWindow) == true)
		loadVisualisationWindow(getVisualisationWidget(l_oVisualisationWindowIdentifier));

	return true;
}

boolean CVisualisationTree::resizeVisualisationPanel(::GtkTreeIter* pIter)
{
	EVisualisationTreeNode l_oType = (EVisualisationTreeNode)getULongValueFromTreeIter(pIter, EVisualisationTreeColumn_ULongNodeType);
	::GtkTreeIter l_oChildIter;

	if(l_oType == EVisualisationTreeNode_VisualisationPanel)
	{
		if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oChildIter, pIter, 0) != FALSE)
			resizeVisualisationPanel(&l_oChildIter);
	}
	else if(l_oType == EVisualisationTreeNode_VerticalSplit || l_oType == EVisualisationTreeNode_HorizontalSplit)
	{
		void* l_pWidget = NULL;
		getPointerValueFromTreeIter(pIter, l_pWidget, EVisualisationTreeColumn_PointerWidget);
		::GtkPaned* l_pPaned = GTK_PANED(getVisualisationWidget(GTK_WIDGET(l_pWidget)));
		if(l_pPaned != NULL)
		{
			//retrieve visualisation widget
			CIdentifier l_oVisualisationWidgetIdentifier;
			getIdentifierFromTreeIter(pIter, l_oVisualisationWidgetIdentifier, EVisualisationTreeColumn_StringIdentifier);
			IVisualisationWidget* l_pVisualisationWidget = getVisualisationWidget(l_oVisualisationWidgetIdentifier);

			//resize it
			resizePanedWidget(l_pVisualisationWidget, GTK_WIDGET(l_pPaned));

			//go down children
			if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oChildIter, pIter, 0) != FALSE)
				resizeVisualisationPanel(&l_oChildIter);
			if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oChildIter, pIter, 1) != FALSE)
				resizeVisualisationPanel(&l_oChildIter);
		}
	}
	else
	{
		return false;
	}

	return true;
}

boolean CVisualisationTree::createTreeWidget(IVisualisationWidget* pVisualisationWidget)
{
	if(m_pTreeViewCB != NULL)
	{
		m_pTreeViewCB->createTreeWidget(pVisualisationWidget);
		return true;
	}

	return false;
}

::GtkWidget* CVisualisationTree::loadTreeWidget(IVisualisationWidget* pWidget)
{
	return m_pTreeViewCB == NULL ? NULL : m_pTreeViewCB->loadTreeWidget(pWidget);
}

::GtkWidget* CVisualisationTree::getVisualisationWidget(::GtkWidget* treeWidget)
{
	return m_pTreeViewCB == NULL ? NULL : m_pTreeViewCB->getVisualisationWidget(treeWidget);
}

::GtkWidget* CVisualisationTree::getTreeWidget(::GtkWidget* visualisationWidget)
{
	return m_pTreeViewCB == NULL ? NULL : m_pTreeViewCB->getTreeWidget(visualisationWidget);
}

const char* CVisualisationTree::getTreeWidgetIcon(EVisualisationTreeNode type)
{
	return m_pTreeViewCB == NULL ? "" : m_pTreeViewCB->getTreeWidgetIcon(type);
}

boolean CVisualisationTree::resizePanedWidget(IVisualisationWidget* pVisualisationWidget, ::GtkWidget* pPanedWidget)
{
	if(m_pTreeViewCB != NULL)
	{
		m_pTreeViewCB->resizePanedWidget(pVisualisationWidget, pPanedWidget);
		return true;
	}

	return false;
}

//Tree helper functions
//---------------------

boolean CVisualisationTree::getTreeSelection(::GtkTreeView* pTreeView, ::GtkTreeIter* pIter)
{
	::GtkTreeSelection* l_pTS = gtk_tree_view_get_selection(pTreeView);
	::GtkTreeModel* l_pTM = GTK_TREE_MODEL(m_pTreeStore);
	return gtk_tree_selection_get_selected(l_pTS, &l_pTM, pIter) != 0;
}

::GtkTreePath* CVisualisationTree::getTreePath(::GtkTreeIter* pTreeIter) const
{
	return (pTreeIter == NULL) ? NULL : gtk_tree_model_get_path(GTK_TREE_MODEL(m_pTreeStore), pTreeIter);
}

unsigned long	CVisualisationTree::getULongValueFromTreeIter(::GtkTreeIter* pTreeIter, EVisualisationTreeColumn col) const
{
	unsigned long l_ulong = 0;
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pTreeIter, col, &l_ulong, -1);
	return l_ulong;
}

boolean CVisualisationTree::getStringValueFromTreeIter(::GtkTreeIter* pTreeIter, char*& rString, EVisualisationTreeColumn col) const
{
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pTreeIter, col, &rString, -1);
	return true;
}

boolean CVisualisationTree::getPointerValueFromTreeIter(::GtkTreeIter* pTreeIter, void*& rPointer, EVisualisationTreeColumn col) const
{
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pTreeIter, col, &rPointer, -1);
	return true;
}

boolean CVisualisationTree::getIdentifierFromTreeIter(::GtkTreeIter* pIter, CIdentifier& rIdentifier, EVisualisationTreeColumn col) const
{
	char* l_pStringIdentifier = NULL;
	getStringValueFromTreeIter(pIter, l_pStringIdentifier, col);
	rIdentifier.fromString(CString(l_pStringIdentifier));
	return true;
}

//looks for a tree node named 'label' of class 'type' from tree root
boolean CVisualisationTree::findChildNodeFromRoot(::GtkTreeIter* pIter, const char* label, EVisualisationTreeNode type)
{
	if(label == NULL)
		return false;

	//if tree is empty return false
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(m_pTreeStore), pIter) == false)
		return false;

	//look for node in the whole tree
	do
	{
		//look for node in current subtree
		if(findChildNodeFromParent(pIter, label, type) == true)
			return true;

		//proceed with next top-level node
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(m_pTreeStore), pIter) != 0);

	//node wasn't found
	return false;
}

//looks for a tree node named 'label' of class 'type' from parent passed as parameter
boolean	CVisualisationTree::findChildNodeFromParent(::GtkTreeIter* pIter, const char* label, EVisualisationTreeNode type)
{
	if(_findChildNodeFromParent(pIter, label, type) == true)
	{
		*pIter = m_oInternalTreeNode;
		return true;
	}
	return false;
}

//looks for a tree node named 'label' of class 'type' from parent passed as parameter
boolean	CVisualisationTree::_findChildNodeFromParent(::GtkTreeIter* pIter, const char* label, EVisualisationTreeNode type)
{
	gchar* l_sName;
	EVisualisationTreeNode l_ulType;

	//is current node the one looked for?
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pIter,
		EVisualisationTreeColumn_StringName, &l_sName,
		EVisualisationTreeColumn_ULongNodeType, &l_ulType,
		-1);

	if(strcmp(label, l_sName) == 0 && type == l_ulType)
	{
		m_oInternalTreeNode = *pIter;
		return true;
	}

	//look among current node's children
	int l_iNbChildren = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(m_pTreeStore), pIter);
	::GtkTreeIter l_oChildIter;
	for(int i=0; i<l_iNbChildren; i++)
	{
		gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oChildIter, pIter, i);

		if(_findChildNodeFromParent(&l_oChildIter, label, type) == true)
			return true;
	}

	//node wasn't found
	return false;
}

boolean CVisualisationTree::findChildNodeFromRoot(::GtkTreeIter* pIter, void* pWidget)
{
	//if tree is empty return false
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(m_pTreeStore), pIter) == false)
		return false;

	//look for node in the whole tree
	do
	{
		//look for node in current subtree
		if(findChildNodeFromParent(pIter, pWidget) == true)
			return true;

		//proceed with next top-level node
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(m_pTreeStore), pIter) != 0);

	//node wasn't found
	return false;
}

boolean	CVisualisationTree::findChildNodeFromParent(::GtkTreeIter* pIter, void* pWidget)
{
	if(_findChildNodeFromParent(pIter, pWidget) == true)
	{
		*pIter = m_oInternalTreeNode;
		return true;
	}
	return false;
}

boolean	CVisualisationTree::_findChildNodeFromParent(::GtkTreeIter* pIter, void* pWidget)
{
	void* l_pWidget;

	//is current node the one looked for?
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pIter, EVisualisationTreeColumn_PointerWidget, &l_pWidget, -1);
	if(pWidget == l_pWidget)
	{
		m_oInternalTreeNode = *pIter;
		return true;
	}

	//look among current node's children
	int l_iNbChildren = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(m_pTreeStore), pIter);
	::GtkTreeIter l_oChildIter;
	for(int i=0; i<l_iNbChildren; i++)
	{
		gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oChildIter, pIter, i);

		if(_findChildNodeFromParent(&l_oChildIter, pWidget) == true)
			return true;
	}

	//node wasn't found
	return false;
}

boolean CVisualisationTree::findChildNodeFromRoot(::GtkTreeIter* pIter, CIdentifier oIdentifier)
{
	//if tree is empty return false
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(m_pTreeStore), pIter) == false)
		return false;

	//look for node in the whole tree
	do
	{
		//look for node in current subtree
		if(findChildNodeFromParent(pIter, oIdentifier) == true)
			return true;

		//proceed with next top-level node
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(m_pTreeStore), pIter) != 0);

	//node wasn't found
	return false;
}

boolean	CVisualisationTree::findChildNodeFromParent(::GtkTreeIter* pIter, CIdentifier oIdentifier)
{
	if(_findChildNodeFromParent(pIter, oIdentifier) == true)
	{
		*pIter = m_oInternalTreeNode;
		return true;
	}
	return false;
}

boolean	CVisualisationTree::_findChildNodeFromParent(::GtkTreeIter* pIter, CIdentifier oIdentifier)
{
	gchar* l_sIdentifier;
	CIdentifier l_oIdentifier;

	//is current node the one looked for?
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pIter, EVisualisationTreeColumn_StringIdentifier, &l_sIdentifier, -1);
	l_oIdentifier.fromString(CString(l_sIdentifier));
	if(oIdentifier == l_oIdentifier)
	{
		m_oInternalTreeNode = *pIter;
		return true;
	}

	//look among current node's children
	int l_iNbChildren = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(m_pTreeStore), pIter);
	::GtkTreeIter l_oChildIter;
	for(int i=0; i<l_iNbChildren; i++)
	{
		gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_pTreeStore), &l_oChildIter, pIter, i);

		if(_findChildNodeFromParent(&l_oChildIter, oIdentifier) == true)
			return true;
	}

	//node wasn't found
	return false;
}

boolean CVisualisationTree::findParentNode(::GtkTreeIter* pIter, EVisualisationTreeNode type)
{
	EVisualisationTreeNode l_ulType;
	::GtkTreeIter l_oIter;

	//is current node the one looked for?
	gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pIter, EVisualisationTreeColumn_ULongNodeType, &l_ulType, -1);
	if(type == l_ulType)
	{
		return true;
	}
	//look one level higher
	else if(gtk_tree_model_iter_parent(GTK_TREE_MODEL(m_pTreeStore), &l_oIter, pIter) != 0)
	{
		*pIter = l_oIter;
		return findParentNode(pIter, type);
	}
	else //couldn't find desired parent node
	{
		return false;
	}
}

boolean CVisualisationTree::dragDataReceivedOutsideWidgetCB(const CIdentifier& rSrcIdentifier, ::GtkWidget* pDstWidget, EDragDataLocation eLocation)
{
	//retrieve source widget parent
	//-----------------------------
	IVisualisationWidget* l_pSrcVisualisationWidget = getVisualisationWidget(rSrcIdentifier);
	if(l_pSrcVisualisationWidget == NULL)
		return false;
	CIdentifier l_oSrcParentIdentifier = l_pSrcVisualisationWidget->getParentIdentifier();

	//retrieve dest widget and dest widget parent identifiers
	//-------------------------------------------------------
	::GtkTreeIter l_oDstIter;
	if(findChildNodeFromRoot(&l_oDstIter, getTreeWidget(pDstWidget)) == false)
		return false;
	CIdentifier l_oDstIdentifier;
	getIdentifierFromTreeIter(&l_oDstIter, l_oDstIdentifier, EVisualisationTreeColumn_StringIdentifier);
	IVisualisationWidget* l_pDstVisualisationWidget = getVisualisationWidget(l_oDstIdentifier);
	if(l_pDstVisualisationWidget == NULL)
		return false;
	CIdentifier l_oDstParentIdentifier = l_pDstVisualisationWidget->getParentIdentifier();

	//unparent source widget
	//----------------------
	uint32 l_ui32SrcIndex = 0;
	unparentVisualisationWidget(rSrcIdentifier, l_ui32SrcIndex);

	//unparent dest widget
	//--------------------
	uint32 l_ui32DstIndex = 0;
	unparentVisualisationWidget(l_oDstIdentifier, l_ui32DstIndex);

	//create paned widget
	//-------------------
	EVisualisationWidgetType l_oPanedType = (eLocation == EDragData_Top || eLocation == EDragData_Bottom) ?
		EVisualisationWidget_VerticalSplit : EVisualisationWidget_HorizontalSplit;
	CIdentifier l_oPanedIdentifier;
	addVisualisationWidget(
		l_oPanedIdentifier,
		CString(l_oPanedType == EVisualisationWidget_VerticalSplit ? "Vertical split" : "Horizontal split"),
		l_oPanedType,
		l_oDstParentIdentifier, //parent paned to dest widget parent
		l_ui32DstIndex, //put it at the index occupied by dest widget
		OV_UndefinedIdentifier, //no box algorithm for a paned
		2); //2 children
	IVisualisationWidget* l_pPanedVisualisationWidget = getVisualisationWidget(l_oPanedIdentifier);

	//add attributes
	//--------------
	createTreeWidget(l_pPanedVisualisationWidget);

	//reparent widgets
	//----------------
	uint32 l_ui32NewSrcIndex = (eLocation == EDragData_Top || eLocation == EDragData_Left) ? 0 : 1;
	parentVisualisationWidget(rSrcIdentifier, l_oPanedIdentifier, l_ui32NewSrcIndex);
	parentVisualisationWidget(l_oDstIdentifier, l_oPanedIdentifier, 1-l_ui32NewSrcIndex);

	//update ::Gtk tree
	//---------------
	reloadTree();

	return true;
}

boolean CVisualisationTree::dragDataReceivedInWidgetCB(const CIdentifier& rSrcIdentifier, ::GtkWidget* pDstWidget)
{
	//retrieve source widget parent
	//-----------------------------
	IVisualisationWidget* l_pSrcVisualisationWidget = getVisualisationWidget(rSrcIdentifier);
	if(l_pSrcVisualisationWidget == NULL)
		return false;
	CIdentifier l_oSrcParentIdentifier = l_pSrcVisualisationWidget->getParentIdentifier();

	//retrieve dest widget and dest widget parent identifiers
	//-------------------------------------------------------
	::GtkTreeIter l_oDstIter;
	if(findChildNodeFromRoot(&l_oDstIter, getTreeWidget(pDstWidget)) == false)
		return false;
	CIdentifier l_oDstIdentifier;
	getIdentifierFromTreeIter(&l_oDstIter, l_oDstIdentifier, EVisualisationTreeColumn_StringIdentifier);
	IVisualisationWidget* l_pDstVisualisationWidget = getVisualisationWidget(l_oDstIdentifier);
	if(l_pDstVisualisationWidget == NULL)
		return false;
	CIdentifier l_oDstParentIdentifier = l_pDstVisualisationWidget->getParentIdentifier();

	//unparent source widget
	//----------------------
	uint32 l_ui32SrcIndex;
	unparentVisualisationWidget(rSrcIdentifier, l_ui32SrcIndex);

	//destroy, unparent or reparent dest widget
	//-----------------------------------------
	uint32 l_ui32DstIndex;
	//if source widget was unaffected
	if(l_oSrcParentIdentifier == OV_UndefinedIdentifier)
	{
		//if dest widget was dummy, destroy it
		if(l_pDstVisualisationWidget->getType() == EVisualisationWidget_Undefined)
		{
			getVisualisationWidgetIndex(l_oDstIdentifier, l_ui32DstIndex);
			destroyHierarchy(l_oDstIdentifier, true);
		}
		else //dest widget becomes unaffected
		{
			unparentVisualisationWidget(l_oDstIdentifier, l_ui32DstIndex);
		}
	}
	else //source widget was affected
	{
		//unparent dest widget
		unparentVisualisationWidget(l_oDstIdentifier, l_ui32DstIndex);

		//reparent it to source widget parent
		parentVisualisationWidget(l_oDstIdentifier, l_oSrcParentIdentifier, l_ui32SrcIndex);
	}

	//reparent source widget
	//----------------------
	parentVisualisationWidget(rSrcIdentifier, l_oDstParentIdentifier, l_ui32DstIndex);

	//update ::Gtk tree
	//---------------
	reloadTree();

	return true;
}

boolean CVisualisationTree::loadVisualisationWindow(IVisualisationWidget* pVisualisationWindow)
{
	//create new tree node
	::GtkTreeIter l_oIter;
	gtk_tree_store_append(m_pTreeStore, &l_oIter, NULL);

	//create new window
	::GtkWidget* l_pWidget = loadTreeWidget(pVisualisationWindow);

	//set tree node fields
	gtk_tree_store_set(m_pTreeStore, &l_oIter,
		EVisualisationTreeColumn_StringName, (const char*)pVisualisationWindow->getName(),
		EVisualisationTreeColumn_StringStockIcon, getTreeWidgetIcon(EVisualisationTreeNode_VisualisationWindow),
		EVisualisationTreeColumn_ULongNodeType, EVisualisationTreeNode_VisualisationWindow,
		EVisualisationTreeColumn_StringIdentifier, (const char*)pVisualisationWindow->getIdentifier().toString(),
		EVisualisationTreeColumn_PointerWidget, l_pWidget,
		-1);

	if(pVisualisationWindow->getNbChildren() > 0)
	{
		::GtkWidget* l_pNotebook = NULL;

		//for each child
		for(uint32 i=0; i<pVisualisationWindow->getNbChildren(); i++)
		{
			//get child identifier
			CIdentifier l_oChildIdentifier;
			pVisualisationWindow->getChildIdentifier(i, l_oChildIdentifier);

			//create child hierarchy
			loadVisualisationPanel(&l_oIter, l_pNotebook, getVisualisationWidget(l_oChildIdentifier));
		}

		//add notebook to window
		if(l_pWidget != NULL && l_pNotebook != NULL)
			gtk_container_add(GTK_CONTAINER(l_pWidget), l_pNotebook);
	}

	return true;
}

boolean CVisualisationTree::loadVisualisationPanel(::GtkTreeIter* pParentIter, ::GtkWidget*& pNotebook, IVisualisationWidget* pVisualisationWidget)
{
	//create new tree node
	::GtkTreeIter l_oIter;
	gtk_tree_store_append(m_pTreeStore, &l_oIter, pParentIter);

	//create new widget
	if(pNotebook == NULL)
		pNotebook = loadTreeWidget(pVisualisationWidget);

	//set tree node fields
	gtk_tree_store_set(m_pTreeStore, &l_oIter,
		EVisualisationTreeColumn_StringName, (const char*)pVisualisationWidget->getName(),
		EVisualisationTreeColumn_StringStockIcon, getTreeWidgetIcon(EVisualisationTreeNode_VisualisationPanel),
		EVisualisationTreeColumn_ULongNodeType, EVisualisationTreeNode_VisualisationPanel,
		EVisualisationTreeColumn_StringIdentifier, (const char*)pVisualisationWidget->getIdentifier().toString(),
		EVisualisationTreeColumn_PointerWidget, pNotebook,
		-1);

	//retrieve panel child
	CIdentifier l_oChildIdentifier;
	pVisualisationWidget->getChildIdentifier(0, l_oChildIdentifier);

	//create a dummy child if none exists
	if(l_oChildIdentifier == OV_UndefinedIdentifier)
		addVisualisationWidget(
			l_oChildIdentifier,
			"Empty",
			EVisualisationWidget_Undefined,
			pVisualisationWidget->getIdentifier(),
			0,
			OV_UndefinedIdentifier,
			0);

	//load hierarchy
	loadVisualisationWidget(&l_oIter, 0, getVisualisationWidget(l_oChildIdentifier));

	//resize visualisation panel
	resizeVisualisationPanel(&l_oIter);

	return true;
}

boolean CVisualisationTree::loadVisualisationWidget(::GtkTreeIter* pParentIter, uint32 ui32Index, IVisualisationWidget* pVisualisationWidget)
{
	//create new tree node
	::GtkTreeIter l_oIter;
	gtk_tree_store_append(m_pTreeStore, &l_oIter, pParentIter);

	//create new widget
	EVisualisationTreeNode l_oParentType = (EVisualisationTreeNode)getULongValueFromTreeIter(pParentIter, EVisualisationTreeColumn_ULongNodeType);
	EVisualisationTreeNode l_oChildType = EVisualisationTreeNode_Undefined;

	if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationBox)
	{
		l_oChildType = EVisualisationTreeNode_VisualisationBox;
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_HorizontalSplit)
	{
		l_oChildType = EVisualisationTreeNode_HorizontalSplit;
	}
	else if(pVisualisationWidget->getType() == EVisualisationWidget_VerticalSplit)
	{
		l_oChildType = EVisualisationTreeNode_VerticalSplit;
	}

	CString l_pStockIconString = getTreeWidgetIcon(l_oChildType);

	if(pVisualisationWidget->getType() == EVisualisationWidget_VisualisationBox)
	{
		//retrieve pointer to IBox
		const IBox* l_pBox = m_pScenario->getBoxDetails(pVisualisationWidget->getBoxIdentifier());
		//retrieve pointer to box algorithm descriptor
		const IBoxAlgorithmDesc* l_pDesc = dynamic_cast<const IBoxAlgorithmDesc*>(
			m_rKernelContext.getPluginManager().getPluginObjectDescCreating(l_pBox->getAlgorithmClassIdentifier()));
		if(l_pDesc != NULL)
		{
			l_pStockIconString = l_pDesc->getStockItemName();
		}
	}

	::GtkWidget* l_pWidget = loadTreeWidget(pVisualisationWidget);

	//set tree node fields
	gtk_tree_store_set(m_pTreeStore, &l_oIter,
		EVisualisationTreeColumn_StringName, (const char*)pVisualisationWidget->getName(),
		EVisualisationTreeColumn_StringStockIcon, (const char*)l_pStockIconString,
		EVisualisationTreeColumn_ULongNodeType, l_oChildType,
		EVisualisationTreeColumn_StringIdentifier, (const char*)pVisualisationWidget->getIdentifier().toString(),
		EVisualisationTreeColumn_PointerWidget, l_pWidget,
		-1);

	//add widget to parent
	if(l_oParentType == EVisualisationTreeNode_VisualisationPanel)
	{
		//parent widget to notebook as a new page
		void* l_pNotebook = NULL;
		getPointerValueFromTreeIter(pParentIter, l_pNotebook, EVisualisationTreeColumn_PointerWidget);
		char* l_pVisualisationPanelName = NULL;
		getStringValueFromTreeIter(pParentIter, l_pVisualisationPanelName, EVisualisationTreeColumn_StringName);
		gtk_notebook_append_page(GTK_NOTEBOOK(l_pNotebook), l_pWidget, gtk_label_new(l_pVisualisationPanelName));
	}
	else if(l_oParentType == EVisualisationTreeNode_VerticalSplit || l_oParentType == EVisualisationTreeNode_HorizontalSplit)
	{
		//insert widget in parent paned
		::GtkWidget* l_pParentWidget;
		gtk_tree_model_get(GTK_TREE_MODEL(m_pTreeStore), pParentIter, EVisualisationTreeColumn_PointerWidget, &l_pParentWidget, -1);
		::GtkWidget* l_pPaned = getVisualisationWidget(l_pParentWidget);
		if(l_pPaned != NULL && GTK_IS_PANED(l_pPaned))
		{
			if(l_pWidget != NULL)
			{
				if(ui32Index == 0)
				{
					gtk_paned_pack1(GTK_PANED(l_pPaned), l_pWidget, TRUE, TRUE);
				}
				else
				{
					gtk_paned_pack2(GTK_PANED(l_pPaned), l_pWidget, TRUE, TRUE);
				}
			}
		}
	}

	if(m_pTreeViewCB)
	{
		m_pTreeViewCB->endLoadTreeWidget(l_pWidget);
	}

	//load hierarchy
	for(uint32 i=0;  i<pVisualisationWidget->getNbChildren(); i++)
	{
		//get child identifier
		CIdentifier l_oChildIdentifier;
		pVisualisationWidget->getChildIdentifier(i, l_oChildIdentifier);

		//create child hierarchy
		loadVisualisationWidget(&l_oIter, i, getVisualisationWidget(l_oChildIdentifier));
	}

	return true;
}

boolean	CVisualisationTree::setWidgets(const OpenViBE::CString& rVisualisationBoxName, ::GtkWidget* pWidget, ::GtkWidget* pToolbarWidget)
{
	if(m_pTreeViewCB != NULL) 
	{
		return m_pTreeViewCB->setWidgets(rVisualisationBoxName, pWidget, pToolbarWidget);
	}
	else
	{
		return false;
	}
}
