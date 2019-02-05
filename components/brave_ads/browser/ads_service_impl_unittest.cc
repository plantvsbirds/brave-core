/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <map>

#include "base/files/scoped_temp_dir.h"
#include "extensions/browser/test_event_router.h"
#include "brave/components/brave_ads/browser/ads_service.h"
#include "brave/components/brave_ads/browser/ads_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service_impl.h"
#include "brave/components/brave_ads/browser/test_util.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/test/test_browser_thread_bundle.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=AdsServiceTest.*

using namespace brave_ads;
using namespace brave_rewards;
using ::testing::_;

class MockRewardsServiceImpl : public RewardsServiceImpl {
 public:
  MockRewardsServiceImpl()
      : RewardsServiceImpl(/*profile*/ nullptr) {}
};

class AdsServiceTest : public testing::Test {
 public:
  AdsServiceTest() {}
  ~AdsServiceTest() override {}

  AdsService* ads_service_;

 protected:
  void SetUp() override {
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    profile_ = CreateBraveAdsProfile(temp_dir_.GetPath());
    ASSERT_TRUE(profile_.get() != NULL);
    RewardsServiceFactory::GetInstance()
        ->SetServiceForTesting(new MockRewardsServiceImpl());
    ads_service_ = AdsServiceFactory::GetForProfile(profile());
    ASSERT_TRUE(AdsServiceFactory::GetInstance() != NULL);
    ASSERT_TRUE(ads_service() != NULL); 
  }

  void TearDown() override {
    profile_.reset();
  }

  Profile* profile() { return profile_.get(); }
  AdsService* ads_service() { return ads_service_; }

 private:
  content::TestBrowserThreadBundle thread_bundle_;
  std::unique_ptr<Profile> profile_;
  base::ScopedTempDir temp_dir_;
};

TEST_F(AdsServiceTest, IsTestingEnv) {
  ASSERT_FALSE(ads_service_->IsTestingEnv());
}
