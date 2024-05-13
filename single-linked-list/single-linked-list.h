#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <initializer_list>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };
    
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_{node} {}
        
        public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_{other.node_} {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        BasicIterator& operator++() noexcept {
            assert(node_);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator previous_value(*this);
            ++(*this);
            return previous_value;
        }
		
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };
        
public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
    
    SingleLinkedList() {
        size_ = 0;
    }
    
    SingleLinkedList(std::initializer_list<Type> values) {
        SingleLinkedList temp;
        initList(temp, values.begin(), values.end());
        swap(temp);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList temp;
        initList(temp, other.begin(), other.end());
        swap(temp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this == &rhs) {
            return *this;
        }
        
        if (rhs.size_ == 0) {
            Clear();
            return *this;
        }
        
        SingleLinkedList temp(rhs);
        swap(temp);
        
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }
    
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }
    
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        Node* afterInsert = pos.node_->next_node;
        pos.node_->next_node = new Node(value, afterInsert);
        ++size_;

        return Iterator(pos.node_->next_node);
    }

    void PopFront() noexcept {
        assert(head_.next_node != nullptr);
        Node* removingNode = head_.next_node;
        head_.next_node = removingNode->next_node;
        delete removingNode;
        --size_;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
       assert(pos.node_ != nullptr && pos.node_->next_node != nullptr && size_ > 0);
       
       Node* removed = pos.node_->next_node;
       Node* afterRemoved = removed->next_node;

       delete removed;
       pos.node_->next_node = afterRemoved;
       --size_;

       return Iterator(afterRemoved);
    }
        
    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return head_.next_node == nullptr;
    }
    
    void PushFront(const Type& value) {
        Node* newFirstNode = new Node(value, head_.next_node);
        head_.next_node = newFirstNode;
        ++size_;
    }
    
    void Clear() {
        while (head_.next_node != nullptr) {
            Node* temp = head_.next_node;
            head_.next_node = temp->next_node;
            delete temp;
            --size_;
        }
    }

private:
    Node head_;
    size_t size_ = 0;
    
    template <typename Iterator>
    void initList(SingleLinkedList& list, Iterator begin, Iterator end) {
        Node* node = &list.head_;

        for (auto it = begin; it != end; ++it) {
            node->next_node = new Node(*it, nullptr);
            node = node->next_node;

            ++list.size_;
        }
    }
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (&lhs == &rhs) {
        return true;
    }
    
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
} 