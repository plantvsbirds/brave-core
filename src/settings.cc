/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "settings.h"
#include "json_helper.h"

namespace state {

Settings::Settings(ads::AdsClient* ads_client) :
    ads_client_(ads_client),
    settings_state_(new SETTINGS_STATE()) {
}

Settings::~Settings() = default;

bool Settings::LoadJson(const std::string& json) {
  SETTINGS_STATE state;
  if (!LoadFromJson(state, json.c_str())) {
    ads_client_->Log(ads::LogLevel::ERROR, "Failed to parse settings json");
    return false;
  }

  settings_state_.reset(new SETTINGS_STATE(state));

  return true;
}

bool Settings::IsAdsEnabled() const {
  return settings_state_->ads_enabled;
}

std::string Settings::GetAdsLocale() const {
  return settings_state_->ads_locale;
}

uint64_t Settings::GetAdsAmountPerHour() const {
  return std::atoll(settings_state_->ads_amount_hour.c_str());
}

uint64_t Settings::GetAdsAmountPerDay() const {
  return std::atoll(settings_state_->ads_amount_day.c_str());
}

}  // namespace state
