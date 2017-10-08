#pragma once


#include "Win32Error.hh"
#include <windows.h>
#include <memory>
#include <vector>
#include <functional>

namespace wwatch {

	template <class Ty>
	struct Array {
		size_t size;

#pragma warning( push )
#pragma warning( disable : 4200)
		Ty values[];
#pragma warning( pop )

		auto begin() { return &[0]; }
		auto cbegin() const { return &[size]; }
		auto end() { return &[0]; }
		auto cend() const { return &[size]; }
	};

	struct Buffer {
		OVERLAPPED overlapped;
		Array<char> data;
	};

	typedef std::unique_ptr<Buffer, std::function<void(Buffer*b)>> BufferPtr;

	class BufferPool {
		std::unique_ptr<char> region_;
		std::vector<Buffer*> freeList_;
		size_t size_;
	public:
		BufferPool(int size, int num) : size_(size*num), region_(new char[size*num]), freeList_(num)
		{
			::memset(region_.get(), 0, size_);
			auto arraySize = size - sizeof(Buffer);
			for (int i = 0; i < num; ++i) {
				freeList_[i] = reinterpret_cast<Buffer*>(region_.get() + (size*i));
				freeList_[i]->data.size = arraySize;
			}
		}

		BufferPtr attach(Buffer* b)
		{
			auto del = [&](Buffer* p) {
				if (p != nullptr) {
					freeList_.push_back(p);
				}
			};
			return BufferPtr(b, del);
		}

		auto alloc() {
			auto b = freeList_.back();
			freeList_.pop_back();
			return attach(b);
		}
	};

	struct Completion {
		BufferPtr buffer;
		ULONG_PTR data;
		DWORD numOfBytes;
	};

	class CompletionPort {
		HANDLE completionPort_;
		BufferPool pool_;
	public:
		CompletionPort(DWORD numOfConcurrentThreads = 0) : pool_(2048, 2000) {
			completionPort_ = ::CreateIoCompletionPort(
				INVALID_HANDLE_VALUE,
				NULL,
				0,
				numOfConcurrentThreads);
			if (completionPort_ == INVALID_HANDLE_VALUE)
				throw wwatch::Win32Error::GetLastWin32Error();
		}

		auto buffer() { return pool_.alloc(); }
		CompletionPort(const CompletionPort&) = delete;

		void associate(HANDLE handle, ULONG_PTR data) {
			auto h = ::CreateIoCompletionPort(
				handle,
				completionPort_,
				data,
				0);
			if (completionPort_ != h)
				throw wwatch::Win32Error::GetLastWin32Error();
		}

		void post(Completion&& completion) {
			auto success = ::PostQueuedCompletionStatus(
				completionPort_,
				completion.numOfBytes,
				completion.data,
				&completion.buffer->overlapped);

			if (!success)
				throw wwatch::Win32Error::GetLastWin32Error();

			completion.buffer.release();
		}

		Completion get(DWORD timeoutMs = INFINITE) {
			DWORD numOfBytes;
			ULONG_PTR data;
			LPOVERLAPPED overlapped;
			auto success = ::GetQueuedCompletionStatus(
				completionPort_,
				&numOfBytes,
				&data,
				&overlapped,
				timeoutMs);

			if (!success)
				throw wwatch::Win32Error::GetLastWin32Error();

			auto buffer = pool_.attach(reinterpret_cast<Buffer*>(overlapped));
			return{ std::move(buffer), data, numOfBytes };
		}

		~CompletionPort() {
			if (completionPort_ != INVALID_HANDLE_VALUE)
				::CloseHandle(completionPort_);
		}
	};
}