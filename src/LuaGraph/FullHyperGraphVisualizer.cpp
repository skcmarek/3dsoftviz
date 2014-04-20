#include "LuaGraph/FullHyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"
#include "limits"

Lua::FullHyperGraphVisualizer::FullHyperGraphVisualizer(Data::Graph *graph)
    : Lua::LuaGraphVisualizer(graph){}

void Lua::FullHyperGraphVisualizer::visualize()
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value(), osg::Vec4f(1,0,0,1), 8);
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value(), osg::Vec4f(1,1,1,1), 1);
    }

    for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = g->getIncidences()->begin(); i != g->getIncidences()->end(); ++i){
        osg::ref_ptr<Data::Node> incNode = currentGraph->addNode(i.key(), i.value()->getLabel(), nodeType);

        setNodeParams(incNode, i.value(), osg::Vec4f(1,1,1,1), 1);

        osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().first);
        osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().second);

        if (i.value()->getOriented()){
            if (i.value()->getOutGoing()){
                currentGraph->addEdge(i.value()->getLabel(), srcNode, incNode, edgeType, true);
                currentGraph->addEdge(i.value()->getLabel(), incNode, dstNode, edgeType, true);
            } else {
                currentGraph->addEdge(i.value()->getLabel(), dstNode, incNode, edgeType, true);
                currentGraph->addEdge(i.value()->getLabel(), incNode, srcNode, edgeType, true);
            }
        } else {
            currentGraph->addEdge(i.value()->getLabel(), srcNode, incNode, edgeType, false);
            currentGraph->addEdge(i.value()->getLabel(), incNode, dstNode, edgeType, false);
        }
    }

    g->setObserver(this);

    QString metaNodeName = "metaNode";
    QString metaEdgeName = "metaEdge";
    osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode(std::numeric_limits<qlonglong>::max(),metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3(0, 0, 1000));

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
        if (i.value()->getParams()["root"]== true){
            osg::ref_ptr<Data::Node> root = currentGraph->getNodes()->value(i.key());
            currentGraph->addEdge(metaEdgeName, root, filesAnchor, currentGraph->getEdgeMetaType(), false);
            break;
        }
    }
}

void Lua::FullHyperGraphVisualizer::onUpdate()
{
    std::cout << "Graph update called" << std::endl;
}

