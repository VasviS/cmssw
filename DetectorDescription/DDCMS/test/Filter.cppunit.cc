#include <cppunit/extensions/HelperMacros.h>

#include "DetectorDescription/DDCMS/interface/Filter.h"
#include <memory>
#include <string_view>
#include <vector>

#include "cppunit/TestAssert.h"
#include "cppunit/TestFixture.h"

using namespace cms;
using namespace cms::dd;
using namespace std;
using namespace std::literals;

class testFilter : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(testFilter);
  CPPUNIT_TEST(checkFilter);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() override;
  void tearDown() override {}
  void checkFilter();

private:
  vector<unique_ptr<Filter>> filters_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(testFilter);

void testFilter::setUp() {
  vector<string_view> selections = {"//MB2P.*",
                                    "//MB2P.*/MB2SuperLayerPhi",
                                    "//MB2P.*/MB2SuperLayerPhi/MB2SLPhiLayer_.*Cells.*",
                                    "//MB2P.*/MB2SuperLayerPhi/MB2SLPhiLayer_.*Cells.*/MBSLPhiGas",
                                    "//MB2P.*/MB2SuperLayerZ",
                                    "//MB2P.*/MB2SuperLayerZ/MB2SLZLayer_.*Cells",
                                    "//MB2P.*/MB2SuperLayerZ/MB2SLZLayer_.*Cells/MB2SLZGas"};
  Filter* currentFilter = nullptr;

  for (const auto& i : selections) {
    vector<string_view> toks = split(i, "/");
    unique_ptr<Filter> f = nullptr;
    auto const& filter = find_if(begin(filters_), end(filters_), [&](auto const& f) {
      auto const& k = find_if(begin(f->keys), end(f->keys), [&](auto const& p) {
        return std::regex_match(std::string({toks.front().data(), toks.front().size()}), p);
      });
      if (k != end(f->keys)) {
        currentFilter = f.get();
        return true;
      }
      return false;
    });
    if (filter == end(filters_)) {
      filters_.emplace_back(unique_ptr<Filter>(
          new Filter{{std::regex(std::string(toks.front().data(), toks.front().size()))}, nullptr, nullptr}));
      currentFilter = filters_.back().get();
    }
    // all next levels
    for (size_t pos = 1; pos < toks.size(); ++pos) {
      if (currentFilter->next != nullptr) {
        currentFilter = currentFilter->next.get();
        auto const& l = find_if(begin(currentFilter->keys), end(currentFilter->keys), [&](auto const& p) {
          return std::regex_match(std::string({toks[pos].data(), toks[pos].size()}), p);
        });
        if (l == end(currentFilter->keys)) {
          currentFilter->keys.emplace_back(std::regex(std::string(toks.front().data(), toks.front().size())));
        }
      } else {
        currentFilter->next.reset(
            new Filter{{std::regex(std::string({toks[pos].data(), toks[pos].size()}))}, nullptr, currentFilter});
      }
    }
  }
}

void testFilter::checkFilter() {
  string_view name = "MB2P.*"sv;
  CPPUNIT_ASSERT(std::regex_match(std::string({name.data(), name.size()}), filters_.front()->keys.front()) == 1);
  CPPUNIT_ASSERT(filters_.front()->up == nullptr);
  CPPUNIT_ASSERT(filters_.front()->next != nullptr);
  CPPUNIT_ASSERT(filters_.size() == 1);

  Filter* current = nullptr;
  for (auto const& i : filters_) {
    current = i.get();
    do {
      current = current->next.get();
    } while (current != nullptr);
  }
}
