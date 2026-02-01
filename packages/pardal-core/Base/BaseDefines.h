#pragma once

#define pdlNOOP() do{}while(0)

#define pdlNoDiscard [[nodiscard]]
#define pdlMaybeUnused [[maybe_unused]]

#define DeclareNonCopyable(classname) classname ( const classname& ) = delete; classname &operator=(classname &a) = delete
#define DeclareNonMoveable(classname) classname(classname &&) = delete; classname &operator=(classname &&a) = delete

#define DeclareDefaultCopyable(classname) classname(const classname &) = default; classname &operator=(const classname &a) = default
#define DeclareDefaultMoveable(classname) classname(classname &&) = default; classname &operator=(classname &&a) = default

#define DefineGlobalConstexprVariableAccessor(classname, name, value)     inline static constexpr classname name() { return value;}

#define DefineGlobalStaticVariableAccessor(classname, name, value)     inline static classname name() { static classname ret = value;  return value; }

#define DeclareBasicIteratorsToMemberContainer(member_container) \
    auto begin() { return (member_container).begin(); }\
    auto cbegin() const { return (member_container).cbegin(); }\
    auto end() { return (member_container).end(); }\
    auto cend() const { return (member_container).cend(); }

#define DefineEnumMaskOperators(enum_type) \
    constexpr enum_type operator | (enum_type lhs, enum_type rhs) { return static_cast<enum_type>(static_cast<std::underlying_type_t<enum_type>>(lhs) | static_cast<std::underlying_type_t<enum_type>>(rhs));} \
    constexpr enum_type operator & (enum_type lhs, enum_type rhs) { return static_cast<enum_type>(static_cast<std::underlying_type_t<enum_type>>(lhs) & static_cast<std::underlying_type_t<enum_type>>(rhs));} \
    constexpr enum_type operator &= (enum_type& lhs, enum_type other) { lhs = lhs & other; return lhs; }\
    constexpr enum_type operator |= (enum_type& lhs, enum_type other) { lhs = lhs | other; return lhs; }\
    constexpr bool operator! (enum_type lhs) { return static_cast<std::underlying_type_t<enum_type>>(lhs) == 0; }
     
