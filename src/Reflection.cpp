#include <entt_snapshot/Reflection.hpp>

namespace snapshot {

#pragma region reflection

std::string_view
Reflection::name() const
{
  using namespace entt::literals;
  return _type.data("name"_hs)
    .get(entt::meta_handle{})
    .cast<std::string_view>();
}

Reflection::Reflection(entt::meta_type in_type)
  : _type(in_type)
{}

Reflection::Reflection(entt::hashed_string name)
  : _type(entt::resolve(name))
{}

Reflection::Reflection(std::string const& name)
  : Reflection(entt::hashed_string{ name.c_str() })
{}

Reflection::Reflection(std::string_view name)
  : Reflection(entt::hashed_string{ name.data() })
{}

Reflection::Reflection(entt::id_type type_id)
  : _type(entt::resolve(type_id))
{}
Reflection::Reflection(entt::type_info type_info)
  : _type(entt::resolve(type_info))
{}

#pragma endregion // reflection

#pragma region component_reflection

entt::meta_any
ComponentReflection::get(entt::const_handle h) const
{
  try {
    auto res = _reflection.type().invoke(
      GET_CONST_COMPONENT_FN_NAME, entt::meta_handle{}, h);

    if (!res || !std::as_const(res).data()) {
      throw std::runtime_error("Failed to get instance of reflected component");
    }
    return res;
  } catch (std::runtime_error& e) {
    throw std::runtime_error("Failed to get instance of reflected component");
  }
}

entt::meta_any
ComponentReflection::get(entt::handle h) const
{
  try {
    auto res =
      _reflection.type().invoke(GET_COMPONENT_FN_NAME, entt::meta_handle{}, h);
    if (!res) {
      throw std::runtime_error("Failed to get instance of reflected component");
    }
    return res;
  } catch (std::runtime_error& e) {
    throw std::runtime_error("Failed to get instance of reflected component");
  }
}

bool
ComponentReflection::contains(entt::const_handle h) const
{
  return _reflection.type()
    .invoke(CONTAINS_COMPONENT_FN_NAME, entt::meta_handle{}, h)
    .cast<bool>();
}

void
ComponentReflection::remove(entt::handle h) const
{
  _reflection.type().invoke(REMOVE_COMPONENT_FN_NAME, entt::meta_handle{}, h);
}

void
ComponentReflection::emplace(entt::handle h, entt::meta_handle comp) const
{
  if (!comp->invoke(EMPLACE_COMPONENT_FN_NAME, h, comp->data())) {
    throw std::runtime_error("Failed to emplace component_any");
  }
}

void
ComponentReflection::emplace(entt::handle h) const
{
  auto comp = _reflection.type().construct();
  if (!comp) {
    throw std::runtime_error(
      "Failed to construct reflected component for emplacement");
  }

  emplace(h, comp);
}

ComponentReflection::ComponentReflection(Reflection in_reflection)
  : _reflection(in_reflection)
{}

#pragma endregion // component_reflection

#pragma region handle

void
Handle::doSave(OutputArchive archive) const
{
  auto data = std::as_const(any)->data();
  if (data == nullptr) {
    throw std::runtime_error("Trying to save nullptr any");
  }
  if (!any->invoke(SAVE_FN_NAME, data, archive)) {
    throw std::runtime_error("Failed to save any");
  }
}

Handle::Handle(entt::meta_any const& any)
  : any(any)
{}

#pragma endregion // handle

#pragma region any

void
Any::doLoad(InputArchive archive)
{
  if (!any.invoke(LOAD_FN_NAME, any.data(), archive)) {
    throw std::runtime_error("Failed to load any");
  }
}

Any::Any(entt::meta_any any)
  : any(any)
{}

#pragma endregion // any

} // namespace snapshot
