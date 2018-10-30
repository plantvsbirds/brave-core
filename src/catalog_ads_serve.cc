/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catalog_ads_serve.h"
#include "static_values.h"
#include "catalog.h"
#include "bundle.h"

namespace catalog {

AdsServe::AdsServe(
    rewards_ads::AdsImpl* ads,
    ads::AdsClient* ads_client,
    std::shared_ptr<state::Bundle> bundle) :
      ads_(ads),
      ads_client_(ads_client),
      bundle_(bundle) {
  BuildUrl();
}

AdsServe::~AdsServe() = default;

void AdsServe::BuildUrl() {
  ads::ClientInfo client_info;
  ads_client_->GetClientInfo(client_info);

  url_ = ads::_is_production ? ADS_PRODUCTION_SERVER : ADS_STAGING_SERVER;

  url_ += "?braveVersion=" + client_info.application_version;
  url_ += "&platform=" + client_info.platform;
  url_ += "&platformVersion=" + client_info.platform_version;
}

void AdsServe::DownloadCatalog() {
  auto url_session = ads_client_->URLSessionTask(
    url_,
    {},
    "",
    "",
    ads::URLSession::Method::GET,
    std::bind(
      &AdsServe::OnCatalogDownloaded,
      this,
      std::placeholders::_1,
      std::placeholders::_2,
      std::placeholders::_3));
}

void AdsServe::OnCatalogDownloaded(
    const int response_status_code,
    const std::string& response,
    const std::map<std::string, std::string>& headers) {
  if (response_status_code / 100 == 2) {
    // TODO(Terry Mancey): Implement Log (#44)
    // 'Catalog downloaded', [ 'version', 'catalog', 'status' ]

    auto catalog = std::make_unique<state::Catalog>(ads_client_);
    if (catalog->LoadJson(response)) {
      auto catalog_state = catalog->GetCatalogState();
      if (bundle_->GenerateFromCatalog(catalog_state)) {
        ads_client_->SaveCatalog(response, this);
        ads_->ApplyCatalog();

        UpdateNextCatalogCheck();

        // TODO(Terry Mancey): Implement Log (#44)
        // 'Catalog parsed', underscore.extend(underscore.clone(header),
        // { status: 'processed', campaigns: underscore.keys(campaigns).length,
        // creativeSets: underscore.keys(creativeSets).length }

        return;
      }
    }
  } else if (response_status_code == 304) {
    UpdateNextCatalogCheck();

    // TODO(Terry Mancey): Implement Log (#44)
    // 'Catalog current', { method, server, path }
  } else {  // failed, so retry
    ads_->StartCollectingActivity(rewards_ads::_one_hour_in_seconds);

    // TODO(Terry Mancey): Implement Log (#44)
    // 'Catalog download failed', { error, method, server, path }
  }
}

void AdsServe::ResetNextCatalogCheck() {
  next_catalog_check_ = 0;
}

void AdsServe::UpdateNextCatalogCheck() {
  // TODO(Terry Mancey): GetPing can no longer come from temporary catalog_
  // next_catalog_check_ = catalog_->GetPing();
  ads_->StartCollectingActivity(next_catalog_check_);
}

//////////////////////////////////////////////////////////////////////////////

void AdsServe::OnCatalogSaved(const ads::Result result) {
}

}  // namespace catalog
