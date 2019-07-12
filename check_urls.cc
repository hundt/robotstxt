// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <fstream>
#include <iostream>

#include "absl/strings/ascii.h"

#include "robots.h"

bool LoadFile(const std::string& filename, std::string* result) {
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    size_t size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), size);
    file.close();
    if (!file) return false;  // file reading error (failbit or badbit).
    result->assign(buffer.begin(), buffer.end());
    return true;
  }
  return false;
}

void ShowHelp(int argc, char** argv) {
    std::cerr << "Shows whether the given user_agent and URI combination"
              << " is allowed or disallowed by the given robots.txt file. "
              << std::endl << std::endl;
    std::cerr << "Usage: " << std::endl << "  " << argv[0]
              << " <robots.txt filename> <user_agent> <URI file>"
              << std::endl << std::endl;
    std::cerr << "The URIs must be %-encoded according to RFC3986."
              << std::endl << std::endl;
    std::cerr << "Example: " << std::endl
              << "  " << argv[0] << " robots.txt FooBot /path/to/uris"
              << std::endl;
}

int main(int argc, char** argv) {
  std::string filename = argc >= 2 ? argv[1] : "";
  if (filename == "-h" || filename == "-help" || filename == "--help") {
    ShowHelp(argc, argv);
    return 0;
  }
  if (argc != 4) {
    std::cerr << "Invalid amount of arguments. Showing help."
              << std::endl << std::endl;
    ShowHelp(argc, argv);
    return 1;
  }
  std::string robots_content;
  if (!(LoadFile(filename, &robots_content))) {
    std::cerr << "failed to read file \"" << filename << "\"" << std::endl;
    return 1;
  }

  std::string user_agent = argv[2];
  std::vector<std::string> user_agents(1, user_agent);
  googlebot::RobotsMatcher matcher;
  std::ifstream url_file(argv[3]);
  if (url_file.rdstate() & std::ifstream::failbit) {
    std::cerr << "failed to read URL file" << std::endl;
    return 1;
  }
  for (std::string url; getline(url_file, url); ) {
    absl::StripAsciiWhitespace(&url);
    bool allowed = matcher.AllowedByRobots(robots_content, &user_agents, url);
    std::cout << (+allowed) << " " << url << std::endl;
  }
}
