#pragma once

#include <entt_snapshot/include_proxy/cereal.hpp>

namespace snapshot {

class OutputArchive
{
public:
  template<typename... TArgs>
  void operator()(TArgs&&... args)
  {
    if (binary) {
      binary->operator()(std::forward<TArgs>(args)...);

    } else {
      json->operator()(std::forward<TArgs>(args)...);
    }
  }

  OutputArchive(cereal::BinaryOutputArchive& binary);
  OutputArchive(cereal::JSONOutputArchive& json);

private:
  cereal::BinaryOutputArchive* binary;
  cereal::JSONOutputArchive* json;
};

class InputArchive
{
public:
  template<typename... TArgs>
  void operator()(TArgs&&... args)
  {
    if (binary) {
      binary->operator()(std::forward<TArgs>(args)...);
    } else {
      json->operator()(std::forward<TArgs>(args)...);
    }
  }

  InputArchive(cereal::BinaryInputArchive& binary);
  InputArchive(cereal::JSONInputArchive& json);

private:
  cereal::BinaryInputArchive* binary;
  cereal::JSONInputArchive* json;
};

class Archive
{
public:
  template<typename... TArgs>
  void operator()(TArgs&&... args)
  {
    if (binary_out) {
      binary_out->operator()(std::forward<TArgs>(args)...);
    } else if (json_out) {
      json_out->operator()(std::forward<TArgs>(args)...);
    } else if (binary_in) {
      binary_in->operator()(std::forward<TArgs>(args)...);
    } else {
      json_in->operator()(std::forward<TArgs>(args)...);
    }
  }

  Archive(cereal::BinaryOutputArchive&);
  Archive(cereal::JSONOutputArchive&);
  Archive(cereal::BinaryInputArchive&);
  Archive(cereal::JSONInputArchive&);

private:
  void setNull();

private:
  cereal::BinaryOutputArchive* binary_out;
  cereal::JSONOutputArchive* json_out;
  cereal::BinaryInputArchive* binary_in;
  cereal::JSONInputArchive* json_in;
};

} // namespace snapshot