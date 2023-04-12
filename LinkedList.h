template<typename T>
class DoubleList {
private:
    struct Node {
        T* data;
        Node* next, * prev;
        Node(T* data, Node* next = nullptr, Node* prev = nullptr) {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };
    Node* _head, * _tail;
    int _sz;
    bool _destructible;
public:
    DoubleList(bool destructible = true) {
        _head = nullptr;
        _tail = nullptr;
        _sz = 0;
        _destructible = destructible;
    }
    ~DoubleList() {
        Node* next = _head;

        while (next != nullptr) {
            Node* curr = next;
            next = next->next;
            if (_destructible)
                delete curr->data;
            delete curr;
        }
        _head = _tail = nullptr;
        _sz = 0;
    }
    void clear() {
        Node* next = _head;

        while (next != nullptr) {
            Node* curr = next;
            next = next->next;
            if (_destructible)
                delete curr->data;
            delete curr;
        }
        _head = _tail = nullptr;
        _sz = 0;
    }
    int size() {
        return _sz;
    }
    void push_back(T* data) {
        if (_head == nullptr) {
            _head = _tail = new Node(data);
        }
        else {
            _tail->next = new Node(data, nullptr, _tail);
            _tail = _tail->next;
        }
        ++_sz;
    }
    void push_back(Node* data) {
        if (_head == nullptr) {
            data->prev = data->next = nullptr;
            _head = _tail = data;
        }
        else {
            _tail->next = data;
            data->prev = _tail;
            data->next = nullptr;
            _tail = data;
        }
        ++_sz;
    }
    void push_front(T* data) {
        if (_head == nullptr) {
            _head = _tail = new Node(data);
        }
        else {
            _head->prev = new Node(data, _head, nullptr);
            _head = _head->prev;
        }
        ++_sz;
    }
    void push_front(Node* data) {
        if (_head == nullptr) {
            data->prev = data->next = nullptr;
            _head = _tail = data;
        }
        else {
            _head->prev = data;
            data->next = _head;
            data->prev = nullptr;
            _head = data;
        }
        ++_sz;
    }
    void stick(DoubleList<T>* second) {
        if (second->begin() == nullptr) return;
        if (_head == nullptr) {
            _head = second->_head;
            _tail = second->_tail;
            _sz = second->_sz;
            return;
        }
        _tail->next = second->_head;
        second->_head->prev = _tail;
        _tail = second->_tail;
        _sz += second->_sz;
        second->_head = nullptr;
        second->_tail = nullptr;
    }
    void pop_front(bool del) {
        if (_head == nullptr) return;
        Node* next = _head->next;
        if (_destructible && del)
            delete _head->data;
        if (del)
            delete _head;
        _head = next;
        --_sz;
        if (_head == nullptr) _tail = nullptr;
        else {
            _head->prev = nullptr;
        }
    }
    void pop_back(bool del) {
        if (_tail == nullptr) return;
        Node* prev = _tail->prev;
        if (_destructible && del)
            delete _tail->data;
        if (del) delete _tail;
        _tail = prev;
        --_sz;
        if (_tail == nullptr) _head = nullptr;
        else {
            _tail->next = nullptr;
        }
    }
    T* front() {
        return _head == nullptr ? nullptr : _head->data;
    }
    T* back() {
        return _tail == nullptr ? nullptr : _tail->data;
    }
    void insert(Node* pos, T* value) {
        if (pos == nullptr || pos->prev == nullptr) {
            push_front(value);
            return;
        }

        Node* new_el = new Node(value, pos, pos->prev);
        pos->prev->next = new_el;
        pos->prev = new_el;
        ++_sz;
    }
    void insert(Node* pos, Node* value) {
        if (pos == nullptr || pos->prev == nullptr) {
            push_front(value);
            return;
        }

        value->next = pos;
        value->prev = pos->prev;
        pos->prev->next = value;
        pos->prev = value;
        ++_sz;
    }
    Node* begin() {
        return _head;
    }
    void merge(DoubleList<T>* list) {
        Node* next = _head;

        while (next != nullptr) {
            Node* curr = next;

            if (list->begin() != nullptr && *curr->data < * list->front()) {
                auto gay = list->begin();
                list->pop_front(false);
                this->insert(curr, gay);
            }
            else {
                next = next->next;
            }
        }

        this->stick(list);
    }
    void sort() {
        if (_head == nullptr || _head->next == nullptr) return;
        bool prev = _destructible;
        _destructible = false;
        DoubleList<T>* first = new DoubleList<T>(false), * second = new DoubleList<T>(false);
        while (this->_head != nullptr) {
            auto gay = this->begin();
            this->pop_front(false);
            first->push_back(gay);

            if (this->_tail != nullptr) {
                auto gay = _tail;
                this->pop_back(false);
                second->push_front(gay);
            }
        }
        first->sort();
        second->sort();
        first->merge(second);
        while (first->begin() != nullptr) {
            auto gay = first->begin();
            first->pop_front(false);
            push_back(gay);
        }

        delete first;
        delete second;
    }
    void erase(Node* node) {
        if (node == nullptr) return;
        if (node == _head) {
            pop_front(true);
            return;
        }
        if (node == _tail) {
            pop_back(true);
            return;
        }
        if (node->prev != nullptr) node->prev->next = node->next;
        if (node->next != nullptr) node->next->prev = node->prev;
        delete node;
        _sz--;
    }
    Node* at(int i) {
        auto p = this->begin();
        for(int j = 0; j < i; ++j) {
            if (p == nullptr) throw -15;
            p = p->next;
        }
        return p;
    }
};
