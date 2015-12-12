//===-- llvm/Support/ThreadPool.h - A ThreadPool implementation -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a crude C++11 based thread pool.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_THREAD_POOL_H
#define LLVM_SUPPORT_THREAD_POOL_H

#include "llvm/Support/thread.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

namespace llvm {

/// A ThreadPool for asynchronous parallel execution on a defined number of
/// threads.
///
/// The pool keeps a vector of threads alive, waiting on a condition variable
/// for some work to become available.
class ThreadPool {
public:
  using TaskTy = std::function<void()>;

  /// Construct a pool with the number of core available on the system (or
  /// whatever the value returned by std::thread::hardware_concurrency() is).
  ThreadPool();

  /// Construct a pool of \p ThreadCount threads
  ThreadPool(unsigned ThreadCount);

  /// Blocking destructor: the pool will wait for all the threads to complete.
  ~ThreadPool();

  /// Asynchronous submission of a task to the pool. The returned future can be
  /// used to wait for the task to finish and is *non-blocking* on destruction.
  template <typename Function, typename... Args>
  inline std::shared_future<void> async(Function &&F, Args &&... ArgList) {
    auto Task =
        std::bind(std::forward<Function>(F), std::forward<Args...>(ArgList...));
    return asyncImpl(Task);
  }

  /// Asynchronous submission of a task to the pool. The returned future can be
  /// used to wait for the task to finish and is *non-blocking* on destruction.
  template <typename Function>
  inline std::shared_future<void> async(Function &&F) {
    return asyncImpl(F);
  }

  /// Blocking wait for all the threads to complete and the queue to be empty.
  /// It is an error to try to add new tasks while blocking on this call.
  void wait();

private:
  /// Asynchronous submission of a task to the pool. The returned future can be
  /// used to wait for the task to finish and is *non-blocking* on destruction.
  std::shared_future<void> asyncImpl(TaskTy f);

  /// Threads in flight
  std::vector<llvm::thread> Threads;

  /// Tasks waiting for execution in the pool.
  std::queue<std::packaged_task<void()>> Tasks;

  /// Locking and signaling for accessing the Tasks queue.
  std::mutex QueueLock;
  std::condition_variable QueueCondition;

  /// Locking and signaling for job completion
  std::mutex CompletionLock;
  std::condition_variable CompletionCondition;

  /// Keep track of the number of thread actually busy
  std::atomic<unsigned> ActiveThreads;

  /// Signal for the destruction of the pool, asking thread to exit.
  bool EnableFlag;
};
}

#endif // LLVM_SUPPORT_THREAD_POOL_H
