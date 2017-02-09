
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

int wmain(int argc, wchar_t *argv[]) {
  if (argc < 4) {
    std::wcout << L"usage:" << std::endl;
    std::wcout << L"\t" << argv[0] << L" \\dir\\to\\watch extension command"
               << std::endl;
    return 1;
  }

  std::wstring dir(argv[1]);
  auto extensions = wwatch::split(argv[2], L',');

  auto begin = &argv[3];
  auto end = begin + (argc - 3);
  std::vector<std::wstring> args(begin, end);
  auto subcommand = wwatch::concat(args, L" ");

  std::wcout << L"Watching " << dir << std::endl;
  std::wcout << L"For extentions \'." << wwatch::concat(extensions, L"\', \'.")
             << "\'" << std::endl;
  std::wcout << L"Will execute " << subcommand << L" [filename]" << std::endl;

  try {
    wwatch::FileWatcher watcher(dir);
    size_t gi = 0;
    while (true) {
      for (const auto &event : watcher.poll()) {
        handle(++gi, event.action, event.path, subcommand, extensions);
      }
    }
  } catch (const wwatch::Win32Error &error) {
    std::wcerr << L"Error Code: " << error.code() << std::endl;
    std::wcerr << L"Error Message: " << error.message() << std::endl;
    return 2;
  }
}
