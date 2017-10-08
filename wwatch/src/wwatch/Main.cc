

#include "CompletionPort.hh"
#include "FileWatcher.hh"
#include "Util.hh"
#include "Win32Error.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

/* TODO: Things to do
 * Add proper args parsing
 * Add timer delay firing
 *  * make events asynchronous
 *  * buffer events
 */

void handle(size_t global, unsigned action, const std::wstring &name,
            const std::wstring &command,
            const std::vector<std::wstring> &extensions) {
  auto run =
      std::any_of(std::cbegin(extensions), std::cend(extensions),
                  [&](const auto &ext) { return wwatch::endsWith(name, ext); });
  if (run) {
    auto fullCommand = wwatch::concat({command, name}, L" ");
    std::wcout << L"#######" << global << L":(" << std::hex << action
               << std::dec << L") " << name << L" - " << fullCommand
               << std::endl;
    ::_flushall();
    ::_wsystem(fullCommand.data());
  }
}

void pollInternal2(wwatch::CompletionPort& completionPort, HANDLE directory) {
	auto buffer = completionPort.buffer();
	if (::ReadDirectoryChangesW(directory, static_cast<LPVOID>(buffer->data.values),
		(DWORD)buffer->data.size, TRUE, FILE_NOTIFY_CHANGE_SIZE,
		NULL, &buffer->overlapped, NULL) == FALSE) {
		throw wwatch::Win32Error::GetLastWin32Error();
	}
	buffer.release();
}

int wmain(int argc, wchar_t *argv[]) {

	std::vector<std::wstring> margs(&argv[0], &argv[argc]);
	if (margs.size() < 4) {
    std::wcout << L"usage:" << std::endl;
    std::wcout << L"\t" << argv[0] << L" \\dir\\to\\watch extension command"
               << std::endl;
    return 1;
  }

  std::wstring dir(margs[1]);
  auto extensions = wwatch::split(margs[2], L',');

  auto begin = margs.begin()+3;
	auto end = margs.end();
  std::vector<std::wstring> args(begin, end);
  auto subcommand = wwatch::concat(args, L" ");

  std::wcout << L"Watching " << dir << std::endl;
  std::wcout << L"For extentions \'." << wwatch::concat(extensions, L"\', \'.")
             << "\'" << std::endl;
  std::wcout << L"Will execute " << subcommand << L" [filename]" << std::endl;

  try {
		wwatch::CompletionPort completionPort;
    wwatch::FileWatcher watcher(dir);
		completionPort.associate(watcher.directory(), 0);
		size_t gi = 0;
		while (true) {
			pollInternal2(completionPort, watcher.directory());
			auto c = completionPort.get();
			auto s = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(c.buffer->data.values);
			auto events = wwatch::toEvents(s);

			for (const auto &event : events) {
				handle(++gi, event.action, event.path, subcommand, extensions);
			}
			if (c.data == -1) break;
		}
  } catch (const wwatch::Win32Error &error) {
    std::wcerr << L"Error Code: " << error.code() << std::endl;
    std::wcerr << L"Error Message: " << error.message() << std::endl;
    return 2;
  }
}
