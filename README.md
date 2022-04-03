# Purpose

A snapshot implementation for entt.
In constract to the library's internal snapshot-implementation this serializes registries in a "entity-major"
order thus (in the author's oppinion) leading to more human-readable and human-editable output (when serializing to a human-readable archive). As serialization doesn't happen using variadic templates, adding new components doesn't lead to incompatibilities.
These benefits come at the cost of performance, and at the requirement to reflect components which should be serialized.

# Install
This is a conan-library so the usual conan usage applies.
This library depends on a conan-generator for vscode which you can obtain either at [my-fork](https://github.com/LeonFretter/conan-vscodepropertiesgen) or at the [original-repository](https://github.com/mkovalchik/conan-vscodepropertiesgen) (though the latter might not be compatible).


# Usage

For the full reflection of a component call `reflectComponent` passing the component-type and a string-view (the name) as template parameters.
Use Snapshot for saving, and SnapshotLoader for loading of registries or individual handles. Archive is just a slim wrapper around
the different archives that were necessary for me. If you require different ones clone this project and add them ;).
