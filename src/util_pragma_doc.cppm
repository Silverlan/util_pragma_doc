// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <vector>
#include <memory>
#include <optional>
#include <string>
#include <functional>

export module pragma.doc;

import pragma.udm;

namespace pragma::doc {
	std::string get_wiki_url() { return "https://wiki.pragma-engine.com/index.php"; }
};
export namespace pragma {
	namespace detail {
		std::string get_wiki_url(const std::string &fullPath, bool isFunOrMember);
	};
	namespace doc {
		class Collection;
		class EnumSet;
		using PCollection = std::shared_ptr<Collection>;
		using PEnumSet = std::shared_ptr<EnumSet>;
		namespace zerobrane {
			std::string generate_autocomplete_script(const std::vector<pragma::doc::PCollection> &collections);
		};
		namespace luals {
			struct Doc {
				std::stringstream &GetStream(const std::string &name);
				std::unordered_map<std::string, std::stringstream> streams;
			};
			Doc generate_doc(const std::vector<pragma::doc::PCollection> &collections);
		};

		enum class GameStateFlags : uint32_t {
			None = 0u,
			Server = 1u,
			Client = Server << 1u,
			GUI = Client << 1u,

			Shared = Server | Client,
			Any = Shared | GUI
		};
		class BaseCollectionObject {
		  public:
			BaseCollectionObject(const BaseCollectionObject &other);
			BaseCollectionObject &operator=(const BaseCollectionObject &other);
			Collection *GetCollection();
			const Collection *GetCollection() const { return const_cast<BaseCollectionObject *>(this)->GetCollection(); }
			void SetCollection(Collection &col);
			virtual std::string GetFullName() const = 0;
			virtual std::string GetWikiURL() const = 0;
		  protected:
			BaseCollectionObject(const Collection *collection = nullptr);
			friend Collection;
			mutable std::weak_ptr<const Collection> m_collection = {};
		};
		enum class ParameterFormatType : uint8_t { Generic = 0, ZeroBrane };
		struct Variant {
			enum class Flags : uint32_t {
				None = 0u,
				Const = 1u,
				Reference = Const << 1u,
				Pointer = Reference << 1u,
				Volatile = Pointer << 1u,
				Unaligned = Volatile << 1u,

				Struct = Unaligned << 1u,
				Class = Struct << 1u,
				Union = Class << 1u,
				Interface = Union << 1u,
				Enum = Interface << 1u,

				Optional = Enum << 1u,
				SmartPtr = Optional << 1u,

				UnknownType = SmartPtr << 1u,

				Final = UnknownType << 1u,
				Fundamental = Final << 1u,
				Arithmetic = Fundamental << 1u,
				Abstract = Arithmetic << 1u,
				Polymorphic = Abstract << 1u,
				Trivial = Polymorphic << 1u
			};
			std::string name;
			Flags flags = Flags::None;
			std::vector<Variant> typeParameters;
			std::string GetFullType() const;
			bool operator==(const Variant &other) const;
			bool operator!=(const Variant &other) const { return !operator==(other); }

			bool IsOptional() const;
			std::string GetFormattedType(ParameterFormatType formatType = ParameterFormatType::Generic, const std::function<void(const Variant &, std::string &)> &typeTranslator = nullptr) const;
		};
		class Parameter {
		  public:
			enum class Flags : uint32_t { None = 0u, Variadic = 1u, Volatile = Variadic << 1u };
			static Parameter Create(const std::string &name);
			static std::string GetFormattedParameterString(const std::vector<Parameter> &params, ParameterFormatType formatType = ParameterFormatType::Generic, bool includeParameterNames = true, const std::function<void(const Variant &, std::string &)> &typeTranslator = nullptr);
			Parameter(const Parameter &) = default;
			Parameter(Parameter &&) = default;
			Parameter &operator=(const Parameter &) = default;
			Parameter &operator=(Parameter &&) = default;
			bool operator==(const Parameter &other) const;
			bool operator!=(const Parameter &other) const { return !operator==(other); }
			const std::string &GetName() const;
			const Variant &GetType() const;
			Variant &GetType();
			std::string GetFullType() const;
			const std::optional<std::string> &GetDefault() const;
			Flags GetFlags() const;
			GameStateFlags GetGameStateFlags() const;

			void SetName(const std::string &name);
			void SetType(const Variant &type);
			void SetType(Variant &&type);
			void SetDefault(const std::string &def);
			void ClearDefault();
			void SetFlags(Flags flags);
			void SetGameStateFlags(GameStateFlags flags);

			std::string GetFormattedType() const;
		  private:
			friend Collection;
			Parameter() = default;
			std::string m_name;
			Variant m_type;
			std::optional<std::string> m_default = {};
			Flags m_flags = Flags::None;
			GameStateFlags m_gameStateFlags = GameStateFlags::None;
		};
		class Group {
		  public:
			static Group Create();
			Group(const Group &) = default;
			Group(Group &&) = default;
			Group &operator=(const Group &) = default;
			Group &operator=(Group &&) = default;
			const std::string &GetName() const;
		  private:
			friend Collection;
			Group() = default;
			std::string m_name;
		};
		struct Source {
			std::string moduleName;
			std::string fileName;
			uint32_t line = 0;
		};
		class Overload {
		  public:
			static Overload Create();
			Overload(const Overload &) = default;
			Overload(Overload &&) = default;
			Overload &operator=(const Overload &) = default;
			Overload &operator=(Overload &&) = default;
			bool operator==(const Overload &other) const;
			bool operator!=(const Overload &other) const { return !operator==(other); }
			std::vector<Parameter> &GetParameters();
			const std::vector<Parameter> &GetParameters() const { return const_cast<Overload *>(this)->GetParameters(); }
			std::vector<Parameter> &GetReturnValues();
			const std::vector<Parameter> &GetReturnValues() const { return const_cast<Overload *>(this)->GetReturnValues(); }

			const std::optional<Source> &GetSource() const { return m_source; }
			void SetSource(const Source &source) { m_source = source; }
			void ClearSource() { m_source = {}; }

			const std::string &GetURL() const { return m_url; }
			void SetURL(const std::string &url) { m_url = url; }

			void AddParameter(const Parameter &parameter);
			void AddReturnValue(const Parameter &parameter);
		  private:
			Overload() = default;
			std::vector<Parameter> m_parameters;
			std::vector<Parameter> m_returnValues;
			std::optional<Source> m_source {};
			std::string m_url;
		};
		class Function : public BaseCollectionObject {
		  public:
			enum class Type : uint32_t { Function = 0u, Method, Hook };
			enum class Flags : uint32_t { None = 0u, Debug = 1u, Vanilla = Debug << 1u, Deprecated = Vanilla << 1u, AutoGenerated = Deprecated << 1u, DefinedInLua = AutoGenerated << 1u, DefinedInCpp = DefinedInLua << 1u, ConstMethod = DefinedInCpp << 1u };
			struct ExampleCode {
				std::string description;
				std::string code;
			};
			static Function Create(const Collection &collection, const std::string &name);
			Function(const Function &) = default;
			Function(Function &&) = default;
			Function &operator=(const Function &) = default;
			Function &operator=(Function &&) = default;
			bool operator==(const Function &other) const;
			bool operator!=(const Function &other) const { return !operator==(other); }
			const std::string &GetName() const;
			virtual std::string GetFullName() const override;
			const std::string &GetDescription() const;
			virtual std::string GetWikiURL() const override;
			Type GetType() const;
			Flags GetFlags() const;
			GameStateFlags GetGameStateFlags() const;
			std::vector<Overload> &GetOverloads();
			const std::vector<Overload> &GetOverloads() const { return const_cast<Function *>(this)->GetOverloads(); }
			const std::optional<ExampleCode> &GetExampleCode() const;
			const std::string &GetURL() const;
			const std::vector<std::string> &GetRelated() const;
			std::vector<Group> &GetGroups();
			const std::vector<Group> &GetGroups() const { return const_cast<Function *>(this)->GetGroups(); }

			void SetName(const std::string &name);
			void SetDescription(const std::string &desc);
			void SetType(Type type);
			void SetFlags(Flags flags);
			void SetGameStateFlags(GameStateFlags flags);
			void AddOverload(const Overload &overload);
			void SetExampleCode(const ExampleCode &code);
			void ClearExampleCode();
			void SetURL(const std::string &url);
			void AddRelated(const std::string &related);
			void AddGroup(const Group &group);
		  private:
			friend Collection;
			Function(const Collection &collection);
			std::string m_name;
			std::string m_identifier;
			std::string m_description;
			Type m_type = Type::Function;
			Flags m_flags = Flags::None;
			GameStateFlags m_gameStateFlags = GameStateFlags::None;
			std::vector<Overload> m_overloads;
			std::optional<ExampleCode> m_exampleCode = {};
			std::string m_url;
			std::vector<std::string> m_related;
			std::vector<Group> m_groups;
		};
		class Member : public BaseCollectionObject {
		  public:
			enum class Mode : uint32_t { None = 0u, Read = 1u, Write = Read << 1u };
			enum class Flags : uint32_t { None = 0u, AutoGenerated = 1u };
			static Member Create(const Collection &collection, const std::string &name);
			Member(const Member &) = default;
			Member(Member &&) = default;
			Member &operator=(const Member &) = default;
			Member &operator=(Member &&) = default;
			bool operator==(const Member &other) const;
			bool operator!=(const Member &other) const { return !operator==(other); }
			const std::string &GetName() const;
			virtual std::string GetFullName() const override;
			const Variant &GetType() const;
			const std::string &GetDescription() const;
			GameStateFlags GetGameStateFlags() const;
			const std::optional<std::string> &GetDefault() const;
			Mode GetMode() const;
			virtual std::string GetWikiURL() const override;

			Flags GetFlags() const { return m_flags; }
			void SetFlags(Flags flags) { m_flags = flags; }

			void SetName(const std::string &name);
			void SetType(const Variant &type);
			void SetType(Variant &&type);
			void SetDescription(const std::string &desc);
			void SetGameStateFlags(GameStateFlags flags);
			void SetDefault(const std::string &def);
			void ClearDefault();
			void SetMode(Mode mode);
		  private:
			friend Collection;
			Member(const Collection &collection);
			std::string m_name;
			Variant m_type;
			std::string m_description;
			Flags m_flags = Flags::None;
			GameStateFlags m_gameStateFlags = GameStateFlags::None;
			std::optional<std::string> m_default = {};
			Mode m_mode = Mode::None;
		};
		class EnumSet;
		class Enum : public BaseCollectionObject {
		  public:
			enum class Type : uint32_t { Regular = 0u, Bit };
			static Enum Create(const EnumSet &es);
			bool operator==(const Enum &other) const;
			bool operator!=(const Enum &other) const { return operator==(other); }
			const std::string &GetName() const;
			virtual std::string GetFullName() const override;
			const std::string &GetValue() const;
			const std::string &GetDescription() const;
			Type GetType() const;
			GameStateFlags GetGameStateFlags() const;
			virtual std::string GetWikiURL() const override;
			const EnumSet *GetEnumSet() const;
			void SetEnumSet(EnumSet &es);

			void SetName(const std::string &name) { m_name = name; }
			void SetValue(const std::string &value) { m_value = value; }
			void SetDescription(const std::string &desc) { m_description = desc; }
			void SetType(Type type) { m_type = type; }
			void SetGameStateFlags(GameStateFlags flags) { m_gameStateFlags = flags; }
		  private:
			friend Collection;
			Enum(const EnumSet &enumSet);
			std::string m_name;
			std::string m_value;
			std::string m_description;
			Type m_type = Type::Regular;
			GameStateFlags m_gameStateFlags = GameStateFlags::None;
			std::weak_ptr<const EnumSet> m_enumSet = {};
			friend EnumSet;
		};
		class EnumSet : public std::enable_shared_from_this<EnumSet>, public BaseCollectionObject {
		  public:
			enum class Flags : uint32_t { None = 0u, AutoGenerated = 1u };

			EnumSet(const Collection *collection = nullptr);
			static std::shared_ptr<EnumSet> Create(const std::string &name, const Collection *collection = nullptr);
			bool operator==(const EnumSet &other) const;
			bool operator!=(const EnumSet &other) const { return !operator==(other); }
			const std::vector<Enum> &GetEnums() const { return const_cast<EnumSet *>(this)->GetEnums(); }
			std::vector<Enum> &GetEnums();
			const std::string &GetName() const;
			virtual std::string GetFullName() const override;
			const std::string &GetUnderlyingType() const;
			virtual std::string GetWikiURL() const override;

			Flags GetFlags() const { return m_flags; }
			void SetFlags(Flags flags) { m_flags = flags; }

			void ReserveEnums(uint32_t n);
			void AddEnum(const Enum &e);
			void AddEnum(Enum &&e);
		  private:
			friend Collection;
			std::vector<Enum> m_enums;
			std::string m_name;
			std::string m_underlyingType;
			Flags m_flags = Flags::None;
		};
		class Module {
		  public:
			const std::string &GetName() const;
			const std::string &GetTarget() const;
		  private:
			Module() = default;
			std::string m_name;
			std::string m_target;
		};
		class DerivedFrom : public std::enable_shared_from_this<DerivedFrom> {
		  public:
			static std::shared_ptr<DerivedFrom> Create(const std::string &name, DerivedFrom *optParent = nullptr);
			const std::string &GetName() const;
			const std::shared_ptr<DerivedFrom> &GetParent() const;
		  private:
			DerivedFrom() = default;
			std::string m_name;
			std::shared_ptr<DerivedFrom> m_parent = {};
		};
		class Collection : public std::enable_shared_from_this<Collection>, public BaseCollectionObject {
		  public:
			static constexpr uint32_t PDOC_VERSION = 1;
			static constexpr auto PDOC_IDENTIFIER = "PDOC";
			static PCollection Load(const udm::AssetData &data, std::string &outErr);
			static PCollection Create();
			bool operator==(const Collection &other) const;
			bool operator!=(const Collection &other) const { return !operator==(other); }
			bool Save(udm::AssetDataArg outData, std::string &outErr);
			enum class Flags : uint32_t {
				None = 0u,
				Base = 1u,
				Library = Base << 1u,
				Class = Library << 1u,
				AutoGenerated = Class << 1u,
				DefinedInLua = AutoGenerated << 1u,
				DefinedInCpp = DefinedInLua << 1u,
			};

			std::vector<Function> &GetFunctions();
			const std::vector<Function> &GetFunctions() const { return const_cast<Collection *>(this)->GetFunctions(); }
			std::vector<Member> &GetMembers();
			const std::vector<Member> &GetMembers() const { return const_cast<Collection *>(this)->GetMembers(); }
			std::vector<PEnumSet> &GetEnumSets();
			const std::vector<PEnumSet> &GetEnumSets() const { return const_cast<Collection *>(this)->GetEnumSets(); }
			std::vector<std::string> &GetRelated();
			const std::vector<std::string> &GetRelated() const { return const_cast<Collection *>(this)->GetRelated(); }
			std::vector<PCollection> &GetChildren();
			const std::vector<PCollection> &GetChildren() const { return const_cast<Collection *>(this)->GetChildren(); }
			std::vector<std::shared_ptr<DerivedFrom>> &GetDerivedFrom();
			const std::vector<std::shared_ptr<DerivedFrom>> &GetDerivedFrom() const { return const_cast<Collection *>(this)->GetDerivedFrom(); }
			void SetName(const std::string &name);
			const std::string &GetName() const;
			virtual std::string GetFullName() const override;
			const std::string &GetDescription() const;
			const std::string &GetURL() const;
			Flags GetFlags() const;
			const Collection *GetParent() const;
			Collection *GetParent();
			virtual std::string GetWikiURL() const override;
			void StripBaseDefinitionsFromDerivedCollections();
			Collection *FindChildCollection(const std::string_view &name);

			void AddFunction(const Function &function);
			void AddMember(const Member &member);
			void AddEnumSet(const PEnumSet &enumSet);
			void AddRelated(const std::string &related);
			void AddChild(const PCollection &collection);
			void AddDerivedFrom(DerivedFrom &derivedFrom);
			void SetDescription(const std::string &desc);
			void SetURL(const std::string &url);
			void SetFlags(Flags flags);
			void SetParent(Collection *optParent);
		  private:
			Collection(Collection *parent = nullptr);
			void Load(udm::LinkedPropertyWrapper &udmCollection);
			bool LoadFromAssetData(const udm::AssetData &data, std::string &outErr);
			void StripBaseDefinitionsFromDerivedCollections(Collection &root);
			std::string m_identifier;
			std::vector<Function> m_functions;
			std::vector<Member> m_members;
			std::vector<PEnumSet> m_enumSets;
			std::vector<std::shared_ptr<DerivedFrom>> m_derivedFrom;
			std::vector<Module> m_modules;
			std::string m_name;
			std::string m_description;
			Flags m_flags = Flags::None;
			std::string m_url;
			std::vector<std::string> m_related;
			std::vector<PCollection> m_children;
			std::weak_ptr<Collection> m_parent = {};
		};
		using namespace umath::scoped_enum::bitwise;
	};
};
export
{
	namespace umath::scoped_enum::bitwise {
		template<>
		struct enable_bitwise_operators<pragma::doc::GameStateFlags> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::Member::Mode> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::Parameter::Flags> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::Function::Flags> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::Collection::Flags> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::EnumSet::Flags> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::Member::Flags> : std::true_type {};
		template<>
		struct enable_bitwise_operators<pragma::doc::Variant::Flags> : std::true_type {};
	}
};
