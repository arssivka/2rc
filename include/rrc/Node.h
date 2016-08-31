/**
 *  @autor arssivka
 *  @date 8/25/16
 */

#pragma once

#include <chrono>
#include "NodeBase.h"

namespace rrc {
    class RootNode;
    typedef std::shared_ptr<RootNode> RootNodePtr;

    class Node : public NodeBase {
    public:
        std::chrono::steady_clock::duration getMinDuration() const;

        void setMinDuration(const std::chrono::steady_clock::duration& minDuration);

    protected:
        Node(RootNodePtr rootNode);

        RootNodePtr getRootNode() const;

    private:
        RootNodePtr mRootNode;
        std::chrono::steady_clock::duration mMinDuration;
    };

    typedef std::shared_ptr<Node> NodePtr;
}


