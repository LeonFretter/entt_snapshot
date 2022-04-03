#include <entt_snapshot/Reflection.hpp>
#include <entt_snapshot/Snapshot.hpp>

namespace snapshot {

#pragma region snapshot

void
Snapshot::save(OutputArchive archive,
               entt::const_handle h,
               ShouldSerializePred should_serialize)
{
  archive(cereal::make_nvp("e_count", 1UL));
  saveHandle(archive, h, should_serialize);
}

void
Snapshot::save(OutputArchive archive,
               entt::registry const& reg,
               ShouldSerializePred should_serialize)
{
  auto sz = reg.size();

  archive(cereal::make_nvp("e_count", sz));

  for (auto it = reg.data(), last = it + sz; it != last; ++it) {
    auto h = entt::const_handle{ reg, *it };
    saveHandle(archive, h, should_serialize);
  }
}

void
Snapshot::saveHandle(OutputArchive& archive,
                     entt::const_handle h,
                     ShouldSerializePred should_serialize)
{
  auto e = h.entity();

  auto e_serial =
    detail::SerializeHandleEntity{ .e = e,
                                   .components = std::vector<Handle>{} };

  h.visit([&e_serial, &h, &should_serialize](
            entt::id_type type_id,
            entt::basic_sparse_set<entt::entity> const& storage) {
    auto refl_comp = ComponentReflection{ storage.type() };
    if (refl_comp) {

      auto comp_name = refl_comp.reflection().name();
      if (should_serialize(comp_name.data())) {
        e_serial.components.push_back(Handle{ refl_comp.get(h) });
      }
    }
  });

  auto label = std::to_string((size_t)e);
  archive(cereal::make_nvp(label, e_serial));
}

#pragma endregion // snapshot

#pragma region snapshot_loader

void
SnapshotLoader::load(InputArchive archive,
                     entt::handle h,
                     ShouldSerializePred should_serialize)
{
  {
    auto sz = 0UL;
    archive(sz);
  }

  loadHandle(archive, h, should_serialize);
}

void
SnapshotLoader::load(InputArchive archive,
                     entt::registry& reg,
                     ShouldSerializePred should_serialize)
{
  auto sz = 0UL;
  archive(sz);

  for (auto i = 0UL; i < sz; ++i) {
    loadHandle(archive, reg, should_serialize);
  }
}

void
SnapshotLoader::loadHandle(InputArchive archive,
                           entt::registry& reg,
                           ShouldSerializePred should_serialize)
{
  auto serial_e = detail::SerializeEntity{};
  archive(serial_e);

  auto h = entt::handle{ reg, reg.create(serial_e.e) };

  for (auto& comp : serial_e.components) {
    auto comp_name = comp.reflection().name().data();
    if (should_serialize(comp_name)) {
      comp.componentReflection().emplace(h, *comp);
    }
  }
}

void
SnapshotLoader::loadHandle(InputArchive archive,
                           entt::handle h,
                           ShouldSerializePred should_serialize)
{
  auto serial_e = detail::SerializeEntity{};
  archive(serial_e);

  for (auto& comp : serial_e.components) {
    auto comp_name = comp.reflection().name().data();

    if (should_serialize(comp_name)) {
      comp.componentReflection().emplace(h, *comp);
    }
  }
}
} // namespace snapshot