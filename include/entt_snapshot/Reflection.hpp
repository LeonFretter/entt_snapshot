#pragma once

#include <entt/entt.hpp>

#include "Archive.hpp"

namespace snapshot {

constexpr auto LOAD_FN_NAME = entt::hashed_string{ "serialize" };
constexpr auto SAVE_FN_NAME = entt::hashed_string{ "save" };
constexpr auto TYPE_FN_NAME = entt::hashed_string{ "type" };

constexpr auto CONTAINS_COMPONENT_FN_NAME = entt::hashed_string{ "contains" };
constexpr auto REMOVE_COMPONENT_FN_NAME = entt::hashed_string{ "remove" };
constexpr auto GET_CONST_COMPONENT_FN_NAME = entt::hashed_string{ "get_const" };
constexpr auto GET_COMPONENT_FN_NAME = entt::hashed_string{ "get" };
constexpr auto EMPLACE_COMPONENT_FN_NAME = entt::hashed_string{ "emplace" };

class Reflection
{
public:
  std::string_view name() const;
  inline entt::meta_type type() const noexcept { return _type; }

  inline operator bool() const noexcept { return _type.operator bool(); }

  Reflection(entt::meta_type);
  Reflection(entt::hashed_string);
  Reflection(std::string const&);
  explicit Reflection(std::string_view);
  Reflection(entt::id_type);
  Reflection(entt::type_info);

private:
  entt::meta_type _type;
};

class ComponentReflection
{
public:
  // CONSIDER: tryGet method

  entt::meta_any get(entt::const_handle) const;
  entt::meta_any get(entt::handle) const;

  bool contains(entt::const_handle) const;
  void remove(entt::handle) const;
  void emplace(entt::handle, entt::meta_handle comp) const;
  void emplace(entt::handle) const;

  inline operator bool() const noexcept { return _reflection.operator bool(); }

  inline Reflection const& reflection() const { return _reflection; }

  ComponentReflection(Reflection);

private:
  Reflection _reflection;
};

class Handle
{
public:
  template<typename T>
  T const& ref() const
  {
    return std::as_const(any)->data();
  }
  template<typename T>
  T& ref()
  {
    return *static_cast<T*>(any->data());
  }

  Reflection reflection() const { return Reflection{ any->type() }; }
  ComponentReflection componentReflection() const
  {
    return ComponentReflection{ reflection() };
  }

  entt::meta_handle const& operator*() const { return any; }
  entt::meta_handle& operator*() { return any; }

  Handle() = default;
  Handle(entt::meta_any const&);

private:
  friend class cereal::access;
  template<typename Archive>
  void save(Archive& archive) const
  {
    if (any) {
      archive(cereal::make_nvp("has_any", true));
      auto temp_name = std::string{ reflection().name().data() };

      archive(cereal::make_nvp("type", temp_name));
      doSave(archive);
    } else {
      archive(cereal::make_nvp("has_any", false));
    }
  }
  template<typename Archive>
  void load(Archive& archive)
  {
    throw std::runtime_error("Don't load via handle");
  }

  void doSave(OutputArchive archive) const;

private:
  entt::meta_handle any;
};

class Any
{
public:
  template<typename T>
  T const& ref() const
  {
    return std::as_const(any).data();
  }
  template<typename T>
  T& ref()
  {
    return *static_cast<T*>(any.data());
  }

  operator bool() const { return any.operator bool(); }

  Reflection reflection() const { return Reflection{ any.type() }; }
  ComponentReflection componentReflection() const
  {
    return ComponentReflection{ reflection() };
  }

  entt::meta_any const& operator*() const { return any; }
  entt::meta_any& operator*() { return any; }

  Any() = default;
  Any(entt::meta_any);

private:
  friend class cereal::access;
  template<typename Archive>
  void save(Archive& archive) const
  {
    throw std::runtime_error("Don't save via any");
  }
  template<typename Archive>
  void load(Archive& archive)
  {
    auto has_any = false;
    archive(has_any);
    if (has_any) {
      auto name = std::string{};
      archive(name);
      auto refl = Reflection{ name };
      any = refl.type().construct();
      if (!any) {
        throw std::runtime_error("Failed to construct any");
      }

      doLoad(archive);
    }
  }

  void doLoad(InputArchive archive);

private:
  entt::meta_any any;
};

/**
 * Collection of functions to be reflected for components.
 * */
namespace ReflectionFunctions {

template<typename T>
void
doLoad(void* data, InputArchive archive)
{
  auto& comp = *static_cast<T*>(data);
  auto name = Reflection{ entt::resolve<T>() }.name();

  archive(cereal::make_nvp(std::string{ name.data() }, comp));
}

template<typename T>
void
doSave(void const* data, OutputArchive archive)
{
  auto& comp = *static_cast<T const*>(data);
  auto name = Reflection{ entt::resolve<T>() }.name();

  archive(cereal::make_nvp(std::string{ name.data() }, comp));
}

template<typename T>
void
doEmplace(entt::handle handle, void* data)
{
  auto& comp = *static_cast<T*>(data);
  handle.emplace_or_replace<T>(std::move(comp));
}

template<typename T>
void
doRemove(entt::handle h)
{
  if (h.all_of<T>()) {
    h.remove<T>();
  }
}

template<typename T>
bool
doContains(entt::const_handle handle)
{
  return handle.all_of<T>();
}

template<typename T>
T&
doGetComponent(entt::handle h)
{
  auto* v = h.template try_get<T>();
  if (v) {
    return *v;
  } else {
    throw std::runtime_error("doGetComponent: can't get component");
  }
}

template<typename T>
T const&
doGetConstComponent(entt::const_handle h)
{
  auto* v = h.template try_get<T>();
  if (v) {
    return *v;
  } else {
    throw std::runtime_error("doGetConstComponent: can't get component");
  }
}

template<typename T>
entt::id_type
doGetType()
{
  return entt::type_hash<T>().value();
}

template<typename T>
void
reflectComponentFunctions()
{
  entt::meta<T>().template func<&doEmplace<T>>(EMPLACE_COMPONENT_FN_NAME);
  entt::meta<T>().template func<&doRemove<T>>(REMOVE_COMPONENT_FN_NAME);
  entt::meta<T>().template func<&doLoad<T>>(LOAD_FN_NAME);
  entt::meta<T>().template func<&doSave<T>>(SAVE_FN_NAME);
  entt::meta<T>().template func<&doContains<T>>(CONTAINS_COMPONENT_FN_NAME);
  entt::meta<T>().template func<&doGetComponent<T>, entt::as_ref_t>(
    GET_COMPONENT_FN_NAME);
  entt::meta<T>().template func<&doGetConstComponent<T>, entt::as_cref_t>(
    GET_CONST_COMPONENT_FN_NAME);
  entt::meta<T>().template func<&doGetType<T>>(TYPE_FN_NAME);
}

template<typename T, std::string_view const& Str>
void
assignName()
{
  using namespace entt::literals;

  entt::meta<T>().template data<&Str>("name"_hs);
}

template<typename T, std::string_view const& Str>
void
reflectWithName()
{
  entt::meta<T>().type(entt::hashed_string{ Str.data() });
  assignName<T, Str>();
}

} // namespace ReflectionFunctions

/**
 * Reflects serialization, emplace, removal, contains, get, get-type for passed
 * component type.
 * */
template<typename T, std::string_view const& Str>
void
reflectComponent()
{
  using namespace ReflectionFunctions;
  reflectWithName<T, Str>();
  reflectComponentFunctions<T>();
}

} // namespace snapshot