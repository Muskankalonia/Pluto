LMAX Disruptor Paper Summary with Experimental Results

🔍 High-Level Goals

The LMAX Disruptor is a concurrency framework that eliminates the need for locks by using a pre-allocated ring buffer and memory barriers.

Traditional concurrent queues like blocking or linked queues suffer from high latency due to:

Frequent memory allocations

CAS (Compare-And-Swap) operations

Memory barriers

Cache line contention

The disruptor reduces these issues through mechanical sympathy—understanding and leveraging hardware features.

🧠 Modern Processor Considerations

Modern CPUs use store buffers to temporarily hold writes, enabling out-of-order execution.

However, synchronization primitives like CAS and memory barriers force the CPU to flush the store buffer, incurring high performance costs.

Volatile writes on their own do not require flushing the store buffer (as CAS does), but still impose memory ordering restrictions depending on the CPU architecture.

By assigning each thread its own slot in the ring buffer and ensuring single-writer discipline, the disruptor avoids CAS on the fast path.

This leads to fewer memory barriers, reduced cache line bouncing, and lower latency under contention.

🏎️ Performance Takeaways

Both locks and CAS induce store buffer flushes and memory fences, impacting performance.

CAS is cheaper than locks but still incurs significant overhead.

The disruptor design shows that avoiding contention entirely is better than synchronizing efficiently.

Hardware-aware programming can significantly reduce latency by avoiding shared memory hotspots.

⚙️ Experimental Results (Lock vs CAS vs Volatile vs No Sync)

Single thread (no sync):         122 ms
Single thread with lock:        5571 ms
Two threads with lock:         24952 ms
Single thread with CAS:         2206 ms
Two threads with CAS:           3958 ms
Single thread with volatile:     136 ms

🔍 Interpretation

No sync (single-thread): Fastest—no synchronization overhead.

Lock (single-thread): Introduces overhead even without contention due to mutex operations.

Lock (two-thread): Worst performance due to contention, context switching, and kernel involvement.

CAS (single-thread): Overhead from atomic instructions and memory fencing.

CAS (two-thread): Better than lock but slower due to contention and store buffer flushes.

Volatile (single-thread): Slight memory ordering cost, but still very efficient; not safe for multiple writers.

These results affirm the Disruptor’s design: minimize synchronization costs by structuring memory access and control flow to eliminate shared mutable state across threads.
- 