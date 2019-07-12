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
    std::cerr << "Provides comments on a robots.txt file. "
              << std::endl << std::endl;
    std::cerr << "Usage: " << std::endl << "  " << argv[0]
              << " <robots.txt filename>"
              << std::endl;
}

int main(int argc, char** argv) {
  std::string filename = argc >= 2 ? argv[1] : "";
  if (filename == "-h" || filename == "-help" || filename == "--help") {
    ShowHelp(argc, argv);
    return 0;
  }
  if (argc != 2) {
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

  googlebot::RobotsRewriter rewriter;
  googlebot::ParseRobotsTxt(robots_content, &rewriter);
}
