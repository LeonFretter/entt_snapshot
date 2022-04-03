#include <entt_snapshot/Archive.hpp>

namespace snapshot {

#pragma region output_archive

OutputArchive::OutputArchive(cereal::BinaryOutputArchive& binary)
  : binary(&binary)
  , json(nullptr)
{}

OutputArchive::OutputArchive(cereal::JSONOutputArchive& json)
  : binary(nullptr)
  , json(&json)
{}

#pragma endregion // output_archive

#pragma region input_archive

InputArchive::InputArchive(cereal::BinaryInputArchive& binary)
  : binary(&binary)
  , json(nullptr)
{}
InputArchive::InputArchive(cereal::JSONInputArchive& json)
  : binary(nullptr)
  , json(&json)
{}

#pragma endregion // input_archive

#pragma region archive

Archive::Archive(cereal::BinaryOutputArchive& binary_out)
{
  setNull();
  this->binary_out = &binary_out;
}
Archive::Archive(cereal::JSONOutputArchive& json_out)
{
  setNull();
  this->json_out = &json_out;
}
Archive::Archive(cereal::BinaryInputArchive& binary_in)
{
  setNull();
  this->binary_in = &binary_in;
}
Archive::Archive(cereal::JSONInputArchive& json_in)
{
  setNull();
  this->json_in = &json_in;
}

void
Archive::setNull()
{
  binary_out = nullptr;
  json_out = nullptr;
  binary_in = nullptr;
  json_in = nullptr;
}

#pragma endregion // archive

} // namespace snapshot