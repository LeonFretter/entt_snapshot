#pragma once

#include "Archive.hpp"
#include "Reflection.hpp"

namespace snapshot {

namespace detail {

struct SerializeHandleEntity
{
  entt::entity e;
  std::vector<Handle> components;

private:
  friend class cereal::access;
  template<typename Archive>
  void save(Archive& archive) const
  {
    auto sz_e = static_cast<size_t>(e);

    archive(cereal::make_nvp("e", sz_e));
    archive(CEREAL_NVP(components));
  }
  template<typename Archive>
  void load(Archive& archive)
  {
    throw std::runtime_error("Don't load via SerializeHandleEntity");
  }
};

struct SerializeEntity
{
  entt::entity e;
  std::vector<Any> components;

private:
  friend class cereal::access;
  template<typename Archive>
  void save(Archive& archive) const
  {
    throw std::runtime_error("Don't save via SerializeEntity");
  }
  template<typename Archive>
  void load(Archive& archive)
  {
    auto sz_e = 0UL;
    archive(cereal::make_nvp("e", sz_e));
    e = static_cast<entt::entity>(sz_e);

    archive(CEREAL_NVP(components));
  }
};

} // namespace detail

using ShouldSerializePred = std::function<bool(char const*)>;

namespace ShouldSerialize {

inline ShouldSerializePred
tautology()
{
  return [](std::string const&) { return true; };
}

} // namespace ShouldSerialize

class Snapshot
{
public:
  static void save(OutputArchive, entt::const_handle, ShouldSerializePred);
  static void save(OutputArchive, entt::registry const&, ShouldSerializePred);

private:
  static void saveHandle(OutputArchive&,
                         entt::const_handle,
                         ShouldSerializePred);
};

class SnapshotLoader
{
public:
  static void load(InputArchive, entt::handle, ShouldSerializePred);
  static void load(InputArchive, entt::registry&, ShouldSerializePred);

private:
  static void loadHandle(InputArchive, entt::registry&, ShouldSerializePred);
  static void loadHandle(InputArchive, entt::handle, ShouldSerializePred);
};

} // namespace snapshot