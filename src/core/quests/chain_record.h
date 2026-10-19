#pragma once

// 1.13c D2Game+0x33f80: compare quest identity until a null-quest sentinel,
// then prepend. Storage owns allocation and the caller owns the resulting list.
// Ref is a native pointer in the game; serialized replay uses backed memory tokens.
namespace d2::quests {
template<class Storage, class Ref, class Quest>
int prependChain(Storage& storage, Ref& head, Quest quest) {
    for (Ref node = head; node; node = storage.next(node)) {
        const auto value = storage.quest(node);
        if (!value) break;
        if (value == quest) return 0;
    }
    const Ref node = storage.allocate();
    storage.initialize(node, quest, head);
    head = node;
    return 1;
}

template<class Node, class Allocator>
struct NativeChainStorage {
    Allocator allocate;
    auto quest(Node* node) const { return node->pQuestData; }
    Node* next(Node* node) const { return node->pNext; }
    template<class Quest>
    void initialize(Node* node, Quest questData, Node* nextNode) const {
        node->pQuestData = questData;
        node->pNext = nextNode;
    }
};
}
