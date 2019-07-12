// Copyright 1999 Google LLC
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
//
// -----------------------------------------------------------------------------
// File: robots.cc
// -----------------------------------------------------------------------------
//
// Implements expired internet draft
//   http://www.robotstxt.org/norobots-rfc.txt
// with Google-specific optimizations detailed at
//   https://developers.google.com/search/reference/robots_txt

#include "robots.h"

#include <stdlib.h>

#include <cstddef>
#include <iostream>
#include <vector>

#include "absl/base/macros.h"
#include "absl/container/fixed_array.h"
#include "absl/strings/ascii.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"

namespace googlebot {
// Internal helper classes and functions.

RobotsRewriter::RobotsRewriter()
    : seen_agent_(false),
      seen_separator_(false) {
}

RobotsRewriter::~RobotsRewriter() {
}

void RobotsRewriter::HandleRobotsStart() {
  // This is a new robots.txt file, so we need to reset all the instance member
  // variables. We do it in the same order the instance member variables are
  // declared, so it's easier to keep track of which ones we have (or maybe
  // haven't!) done.
  seen_agent_ = false;
  seen_separator_ = false;
  printed_current_agents_ = false;
  current_agents_.clear();
  current_agents_line_nums_.clear();
}

void RobotsRewriter::HandleRobotsEnd() {
  // std::cerr << "HandleRobotsEnd " << "\n";
  this->CheckForIgnoredAgent();
}

/*static*/ absl::string_view RobotsRewriter::ExtractUserAgent(
    absl::string_view user_agent) {
  // Allowed characters in user-agent are [a-zA-Z_-].
  const char* end = user_agent.data();
  while (absl::ascii_isalpha(*end) || *end == '-' || *end == '_') {
    ++end;
  }
  return user_agent.substr(0, end - user_agent.data());
}

void RobotsRewriter::PrintCurrentAgents(bool wasIgnored) {
  if (printed_current_agents_ || !seen_agent_) {
      return;
  }
  for (int i = 0; i < current_agents_.size(); i++) {
    const auto agent = current_agents_[i];
    const auto line_num = current_agents_line_nums_[i];
    std::cerr << line_num;
    if (wasIgnored) {
      std::cerr << " # IGNORED because no rules followed: ";
    }
    std::cerr << " User-agent: " << agent << "\n";
  }
  printed_current_agents_ = true;
}

void RobotsRewriter::CheckForIgnoredAgent() {
  // std::cerr << "CheckForIgnoredAgent " << seen_agent_ << "/" << printed_current_agents_ << "\n";
  if (seen_agent_ && !printed_current_agents_) {
    this->PrintCurrentAgents(true);
  }
}

void RobotsRewriter::HandleUserAgent(int line_num,
                                    absl::string_view user_agent) {
  // std::cerr << "HandleUserAgent " << line_num << " " << user_agent << "\n";
  if (seen_separator_) {
    seen_separator_ = false;
    current_agents_.clear();
    current_agents_line_nums_.clear();
  }
  CheckForIgnoredAgent();
  seen_agent_ = true;

  // Google-specific optimization: a '*' followed by space and more characters
  // in a user-agent record is still regarded a global rule.
  current_agents_line_nums_.push_back(line_num);
  if (user_agent.length() >= 1 && user_agent[0] == '*' &&
      (user_agent.length() == 1 || isspace(user_agent[1]))) {
    current_agents_.push_back("*");
  } else {
    current_agents_.push_back(ExtractUserAgent(user_agent));
  }
  printed_current_agents_ = false;
}

void RobotsRewriter::HandleAllow(int line_num, absl::string_view value) {
  // std::cerr << "HandleAllow " << value << "\n";
  PrintCurrentAgents(false);
  if (!seen_agent_) {
    std::cerr << line_num << " # IGNORED because no user-agent: Allow " << value << "\n";
    return;
  }
  std::cerr << line_num << " Allow " << value << "\n";
  seen_separator_ = true;
}

void RobotsRewriter::HandleDisallow(int line_num, absl::string_view value) {
  // std::cerr << "HandleDisallow " << value << "\n";
  PrintCurrentAgents(false);
  if (!seen_agent_) {
    std::cerr << line_num << " # IGNORED because no user-agent: Disallow " << value << "\n";
    return;
  }
  std::cerr << line_num << " Disallow " << value << "\n";
  seen_separator_ = true;
}

void RobotsRewriter::HandleSitemap(int line_num, absl::string_view value) {
  seen_separator_ = true;
}

void RobotsRewriter::HandleUnknownAction(int line_num, absl::string_view action,
                                        absl::string_view value) {
  seen_separator_ = true;
}

}  // namespace googlebot
