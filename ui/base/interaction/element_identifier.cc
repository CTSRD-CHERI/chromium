// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/interaction/element_identifier.h"

#include <cstring>

#include "base/containers/contains.h"
#include "base/no_destructor.h"

namespace ui {

std::string ElementIdentifier::GetName() const {
  if (!handle_)
    return std::string();
  RegisterKnownIdentifier(*this);
  return handle_->name;
}

#if defined(__CHERI_PURE_CAPABILITY__)
uintptr_t ElementIdentifier::GetRawValue() const {
#else // defined(__CHERI_PURE_CAPABILITY__)
intptr_t ElementIdentifier::GetRawValue() const {
#endif // defined(__CHERI_PURE_CAPABILITY__)
  if (!handle_)
    return 0;
  RegisterKnownIdentifier(*this);
  return reinterpret_cast<uintptr_t>(handle_);
}

// static
#if defined(__CHERI_PURE_CAPABILITY__)
ElementIdentifier ElementIdentifier::FromRawValue(uintptr_t value) {
#else // defined(__CHERI_PURE_CAPABILITY__)
ElementIdentifier ElementIdentifier::FromRawValue(intptr_t value) {
#endif // defined(__CHERI_PURE_CAPABILITY__)
  if (!value)
    return ElementIdentifier();
  const auto* impl =
      reinterpret_cast<const internal::ElementIdentifierImpl*>(value);
  CHECK(base::Contains(GetKnownIdentifiers(), impl));
  return ElementIdentifier(impl);
}

// static
ElementIdentifier ElementIdentifier::FromName(const char* name) {
  for (const auto* impl : GetKnownIdentifiers()) {
    if (!strcmp(impl->name, name))
      return ElementIdentifier(impl);
  }
  return ElementIdentifier();
}

// static
void ElementIdentifier::RegisterKnownIdentifier(
    ElementIdentifier element_identifier) {
  CHECK(element_identifier);

#if DCHECK_IS_ON()
  // Enforce uniqueness in DCHECK builds.
  const ElementIdentifier existing = FromName(element_identifier.handle_->name);
  DCHECK(!existing || existing == element_identifier);
#endif

  GetKnownIdentifiers().insert(element_identifier.handle_);
}

// static
ElementIdentifier::KnownIdentifiers& ElementIdentifier::GetKnownIdentifiers() {
  static base::NoDestructor<KnownIdentifiers> known_identifiers;
  return *known_identifiers.get();
}

void PrintTo(ElementIdentifier element_identifier, std::ostream* os) {
  *os << "ElementIdentifier " << element_identifier.GetRawValue() << " ["
      << element_identifier.GetName() << "]";
}

void PrintTo(ElementContext element_context, std::ostream* os) {
  *os << "ElementContext " << static_cast<const void*>(element_context);
}

extern std::ostream& operator<<(std::ostream& os,
                                ElementIdentifier element_identifier) {
  PrintTo(element_identifier, &os);
  return os;
}

extern std::ostream& operator<<(std::ostream& os,
                                ElementContext element_context) {
  PrintTo(element_context, &os);
  return os;
}

}  // namespace ui
