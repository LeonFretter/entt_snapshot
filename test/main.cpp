
#include <entt/entt.hpp>
#include <gtest/gtest.h>
#include <string_view>

#include <entt_snapshot/Reflection.hpp>

using namespace snapshot;

constexpr std::string_view TEST_COMPONENT_NAME = "test_comp";
constexpr std::string_view OTHER_COMPONENT_NAME = "other_comp";

entt::handle
createHandle(entt::registry& reg)
{
  return entt::handle{ reg, reg.create() };
}

struct TestComponent
{
  size_t some_value;

private:
  friend class cereal::access;
  template<typename Archive>
  void serialize(Archive& archive)
  {
    archive(CEREAL_NVP(some_value));
  }
};

struct OtherComponent
{
  size_t some_other_value;

private:
  friend class cereal::access;
  template<typename Archive>
  void serialize(Archive& archive)
  {
    archive(CEREAL_NVP(some_other_value));
  }
};

TEST(ReflectionTest, haveName)
{
  auto accu_name = std::string{ TEST_COMPONENT_NAME.data() };
  auto refl_accu = Reflection{ accu_name };
  EXPECT_TRUE(refl_accu.operator bool());
  EXPECT_EQ(refl_accu.name(), TEST_COMPONENT_NAME);
}

TEST(ComponentReflectionTest, dontContain)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);

  h.emplace<TestComponent>();
  auto refl = ComponentReflection{ Reflection{ OTHER_COMPONENT_NAME } };
  EXPECT_FALSE(refl.contains(h));
}

TEST(ComponentReflectionTest, contain)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);

  h.emplace<OtherComponent>();
  auto refl = ComponentReflection{ Reflection{ OTHER_COMPONENT_NAME } };
  EXPECT_TRUE(refl.contains(h));
}

TEST(ComopnentReflectionTest, getNonPresent)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);

  auto refl = ComponentReflection{ Reflection{ TEST_COMPONENT_NAME } };

  EXPECT_THROW(refl.get(h), std::runtime_error);
}

TEST(ComponentReflectionTest, get)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);
  h.emplace<TestComponent>(TestComponent{ .some_value = 4UL });

  auto decay_refl = ComponentReflection{ Reflection{ TEST_COMPONENT_NAME } };
  auto refl_instance = decay_refl.get(h);

  EXPECT_EQ(refl_instance.cast<TestComponent>().some_value, 4UL);
}

TEST(ComponentReflectionTest, emplace)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);

  auto refl = ComponentReflection{ Reflection{ TEST_COMPONENT_NAME } };

  refl.emplace(h);
  EXPECT_TRUE(h.all_of<TestComponent>());
}

TEST(ComponentReflectionTest, emplaceOverride)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);
  {
    auto& test_comp = h.emplace<TestComponent>();
    test_comp.some_value = 4.2f;
  }

  auto decay_refl = ComponentReflection{ Reflection{ TEST_COMPONENT_NAME } };

  {
    auto elem = TestComponent{ .some_value = 2UL };
    decay_refl.emplace(h, elem);
  }

  EXPECT_EQ(h.get<TestComponent>().some_value, 2UL);
}

TEST(ComponentReflectionTest, doRemove)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);
  h.emplace<TestComponent>();

  auto decay_refl = ComponentReflection{ Reflection{ TEST_COMPONENT_NAME } };
  decay_refl.remove(h);

  EXPECT_FALSE(h.all_of<TestComponent>());
}

TEST(ComponentReflectionTest, throwOnGetNonPresent)
{
  auto reg = entt::registry{};
  auto h = createHandle(reg);

  auto decay_refl = ComponentReflection{ Reflection{ TEST_COMPONENT_NAME } };
  EXPECT_THROW(decay_refl.get(h), std::runtime_error);
}

TEST(AnyTest, throwOnSave)
{
  auto any = Any{ TestComponent{} };
  auto stream = std::stringstream{};
  auto archive = cereal::BinaryOutputArchive{ stream };

  // for saving, non-owning handles should be used
  EXPECT_THROW(archive(any), std::runtime_error);
}

TEST(HandleTest, handleSaveNull)
{
  auto mh = Handle{};

  auto stream = std::stringstream{};
  auto archive = cereal::BinaryOutputArchive{ stream };
  // should just serialize !has_any
  EXPECT_NO_THROW(archive(mh));
}

TEST(HandleTest, throwOnLoad)
{
  auto elem = TestComponent{};
  auto first_mh = Handle{ elem };
  auto stream = std::stringstream{};
  {
    auto oarchive = cereal::BinaryOutputArchive{ stream };
    EXPECT_NO_THROW(oarchive(first_mh));
  }
  auto iarchive = cereal::BinaryInputArchive{ stream };

  // no non-owning objects to be used for loading
  auto mh = Handle{};
  EXPECT_THROW(iarchive(mh), std::runtime_error);
}

// TODO: add snapshot tests

int
main(int argc, char** argv)
{
  reflectComponent<TestComponent, TEST_COMPONENT_NAME>();
  reflectComponent<OtherComponent, OTHER_COMPONENT_NAME>();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}