#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include <windows.h>


#include <intrin.h>

// Use MSVC intrinsics

// For loads and stores, its not clear if we can rely on register width loads
// and stores to be atomic as reported here
// https://learn.microsoft.com/en-us/windows/win32/sync/interlocked-variable-access?redirectedfrom=MSDN
// or if we have to reuse other instrinsics
// https://stackoverflow.com/questions/42660091/atomic-load-in-c-with-msvc
// We reuse other intrinsics to be cautious
#define atomic_load_u8(a) _InterlockedOr8(a, 0)
#define atomic_load_u16(a) _InterlockedOr16(a, 0)
#define atomic_load_u32(a) _InterlockedOr(a, 0)
#define atomic_load_u64(a) _InterlockedOr64(a, 0)

#define atomic_store_u8(a, v) _InterlockedExchange8(a, v)
#define atomic_store_u16(a, v) _InterlockedExchange16(a, v)
#define atomic_store_u32(a, v) _InterlockedExchange(a, v)
#define atomic_store_u64(a, v) _InterlockedExchange64(a, v)

#define atomic_add_u8(a, v) _InterlockedExchangeAdd8(a, v)
#define atomic_add_u16(a, v) _InterlockedExchangeAdd16(a, v)
#define atomic_add_u32(a, v) _InterlockedExchangeAdd(a, v)
#define atomic_add_u64(a, v) _InterlockedExchangeAdd64(a, v)

#define atomic_sub_u8(a, v) _InterlockedExchangeAdd8(a, -(v))
#define atomic_sub_u16(a, v) _InterlockedExchangeAdd16(a, -(v))
#define atomic_sub_u32(a, v) _InterlockedExchangeAdd(a, -(v))
#define atomic_sub_u64(a, v) _InterlockedExchangeAdd64(a, -(v))

#define atomic_and_u8(a, v) _InterlockedAnd8(a, v)
#define atomic_and_u16(a, v) _InterlockedAnd16(a, v)
#define atomic_and_u32(a, v) _InterlockedAnd(a, v)
#define atomic_and_u64(a, v) _InterlockedAnd64(a, v)

#define atomic_or_u8(a, v) _InterlockedOr8(a, v)
#define atomic_or_u16(a, v) _InterlockedOr16(a, v)
#define atomic_or_u32(a, v) _InterlockedOr(a, v)
#define atomic_or_u64(a, v) _InterlockedOr64(a, v)

#define atomic_xor_u8(a, v) _InterlockedXor8(a, v)
#define atomic_xor_u16(a, v) _InterlockedXor16(a, v)
#define atomic_xor_u32(a, v) _InterlockedXor(a, v)
#define atomic_xor_u64(a, v) _InterlockedXor64(a, v)

#define atomic_exchange_u8(a, v) _InterlockedExchange8(a, v)
#define atomic_exchange_u16(a, v) _InterlockedExchange16(a, v)
#define atomic_exchange_u32(a, v) _InterlockedExchange(a, v)
#define atomic_exchange_u64(a, v) _InterlockedExchange64(a, v)

// clang-format off
#define atomic_compare_exchange_u8(a, expected_ptr, desired) _InterlockedCompareExchange8(a, desired, *(expected_ptr))
#define atomic_compare_exchange_u16(a, expected_ptr, desired) _InterlockedCompareExchange16(a, desired, *(expected_ptr))
#define atomic_compare_exchange_u32(a, expected_ptr, desired) _InterlockedCompareExchange(a, desired, *(expected_ptr))
#define atomic_compare_exchange_u64(a, expected_ptr, desired) _InterlockedCompareExchange64(a, desired, *(expected_ptr))

#define atomic_compare_exchange_ptr(a, expected, desired) _InterlockedCompareExchange64(a, desired, expected)
// clang-format on

#define atomic_fence() _ReadWriteBarrier()

typedef struct {
    void* value;
    wasm2c_atomic_queue_node* next;
} wasm2c_atomic_queue_node;

typedef struct {
    wasm2c_atomic_queue_node storage_dummy_head;
    wasm2c_atomic_queue_node* dummy_head;
    wasm2c_atomic_queue_node* maybe_tail;
} wasm2c_atomic_queue;

wasm2c_atomic_queue wasm2c_atomic_queue_create() {
    wasm2c_atomic_queue q = {0};
    q.dummy_head = &q.storage_dummy_head;
    q.maybe_tail = &q.storage_dummy_head;
    return q;
}

// Enqueue(x)
//     new_node = new record
//     new_node->value = x
//     new_node->next = NULL
//     iter = q.tail
//     old_iter = iter
//     do {
//         while iter->next != NULL
//             iter =iter->next
//     } until (Compare&Swap(iter->next , NULL, new_node ) == SUCCESS)
//     Compare&Swap(q.tail ; old_iter; new_node )

#define atomic_compare_exchange_ptr_succeeded(a, expected, desired) atomic_compare_exchange_ptr(a, expected, desired) == expected

void wasm2c_atomic_queue_enqueue(wasm2c_atomic_queue* q, void* value) {
    wasm2c_atomic_queue_node* new_node = malloc(sizeof(wasm2c_atomic_queue_node));
    new_node->value = value;
    new_node->next = NULL;

    wasm2c_atomic_queue_node* iter = q->maybe_tail;
    wasm2c_atomic_queue_node* old_iter = iter;

    do {
        while (iter->next != NULL) {
            iter = atomic_load_u64(iter->next);
        };

    } while (!atomic_compare_exchange_ptr_succeeded(iter->next, NULL, new_node));

    atomic_compare_exchange_ptr(&(q->maybe_tail), old_iter, new_node);
}

// Dequeue()
//     repeat
//          iter_new = iter
//          if iter_new->next = NULL
//              error queue empty
//     until Compare&Swap(iter ; iter_new; iter_new->next )
//     return iter_new->next->value

wasm2c_atomic_queue_node* wasm2c_atomic_queue_dequeue(wasm2c_atomic_queue* q) {

    wasm2c_atomic_queue_node* iter = q->dummy_head;
    wasm2c_atomic_queue_node* iter_new = NULL;

    do {
        iter_new = iter;
        if (atomic_load_u64(iter_new->next) == NULL) {
            return NULL;
        }
    } while(!atomic_compare_exchange_ptr_succeeded(&iter, iter_new, iter_new->next));
    return iter_new->next->value;
}

#undef atomic_compare_exchange_ptr_succeeded


int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
