#include <atomic>
#include <memory>

template<typename T>
class lock_free_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::atomic<node*> next;
        node(T const& data_):
            data(std::make_shared<T>(data_))
        {}
    };
    std::atomic<node*> head;
    std::atomic<node*> tail;
public:
    void push(T const& data)
    {
        std::atomic<node*> const new_node=new node(data);
        node* old_tail = tail.load();
        while(!old_tail->next.compare_exchange_weak(nullptr, new_node)){
          node* old_tail = tail.load();
        }
        tail.compare_exchange_weak(old_tail, new_node);
    }
    std::shared_ptr<T> pop()
    {
        node* old_head=head.load();
        while(old_head &&
            !head.compare_exchange_weak(old_head,old_head->next)){
            old_head=head.load();
        }
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
};

int main(int argc, char const *argv[])
{
    lock_free_queue<int> a;
    /* code */
    return 0;
}
