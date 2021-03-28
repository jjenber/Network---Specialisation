#pragma once

#define RAPIDJSON_HAS_STDSTRING true

#include "RapidJSON/document.h"
#include "RapidJSON/istreamwrapper.h"
#include <functional>

namespace CommonUtilities
{
	class JsonUtility
	{
	public:
		JsonUtility() = default;
		JsonUtility(const char* aFilePath, bool aCreateDocIfNotFound = false);

		JsonUtility(const JsonUtility& aCopy) = delete;
		JsonUtility(JsonUtility&& aCopy) = delete;
		JsonUtility& operator=(const JsonUtility& aCopy) = delete;
		JsonUtility& operator=(JsonUtility&& aCopy) = delete;

		/// Directly access a value. Throws exception if the member doesn't exist.
		const rapidjson::Value& operator[](const char* aMemberName) const;

		/// Directly access a value. Throws exception if the member doesn't exist.
		rapidjson::Value& operator[](const char* aMemberName);

		bool Open(const char* aFilePath, bool aCreateDocIfNotFound = false);
		
		/// Removes all members. Does not deallocate memory, the capacity stays the same. Call SaveDocument() to save to disk.
		void Clear();

		/// Clears the document and also deletes the file on disc.
		void Delete();
		
		/// Outputs the DOM to a file. Set parameter if the document should be saved elsewhere.
		void SaveDocument(const std::string& aFilePath = std::string());

		/// Finds an array member and calls the callback each iteration and passes the current element.
		/// Use the getter methods on the rapidjson::Value.
		void IterateOverArray(const char* aMemberName, std::function<void(const rapidjson::Value& anElement)> aCallback);

		/// Finds an array member and calls the callback each iteration and passes the current element. 
		/// All elements must be of same type e.g. integers or strings.
		/// Use IterateOverArray() for multiple types.
		template <class ValueType>
		void IterateOverArraySingleType(const char* aMemberName, std::function<void(const ValueType& anElement)> aCallback);

		/// Retrives a value of the template type. E.g ParseValue<int>(myInt). Asserts that member exists.
		template <typename ValueType>
		ValueType ParseValue(const char* aMemberName) const;

		/// Retrives a value of the template type. E.g ParseValue<int>(myInt). Asserts that member exists.
		template <typename ValueType>
		ValueType ParseValue(const char* aMemberName);

		/// Overwrites the value (including type) of an existing member or creates a new member with the value.
		template <class ValueType>
		void WriteValue(const char* aMemberName,const ValueType& aValue);

		/// Overwrites the string of an existing member or creates a new string member.
		template<>
		void WriteValue<const std::string&>(const char* aMemberName,const std::string& aValue);

		/// Overwrites the string of an existing member or creates a new string member.
		template <size_t N>
		void WriteValue(const char* aMemberName, const char(&aValue)[N]);

		/// Removes the specified member from the json document. Call SaveDocument() to save to disk.
		void EraseValue(const char* aMemberName);

		bool HasMember(const char* aMemberName) const;

		/// Returns true if a ParseError has occurred. Call GetParseErrorMessage() to get a string with potential info.
		bool HasError() const;
		
		/// Returns a string with the current ParseErrorMessage. Call HasError() to check if an error exists before calling GetParseErrorMessage. Call GetErrorOffset() to get the position.
		std::string GetParseErrorMessage() const;
		
		/// Returns the position of the last parse error. Returns 0 if no error.
		size_t GetErrorOffset() const;
	
		/// Retrieves the wrapped rapidjson Document. Avoid using this directly.
		rapidjson::Document& GetDocument();

	private:
		/// Returns a string with the mapped error message.
		const char* ParseErrorCode(rapidjson::ParseErrorCode aCode) const;

		rapidjson::Document myJsonDocument;
		std::string myFilePath;
	};

	template<class ValueType>
	inline void JsonUtility::IterateOverArraySingleType(const char* aMemberName, std::function<void(const ValueType& anElement)> aCallback)
	{
		rapidjson::Value::ConstMemberIterator member = myJsonDocument.FindMember(aMemberName);
		if ((member == myJsonDocument.MemberEnd()))
		{
			return;
		}
		if (!member->value.IsArray())
		{
			assert(false && "Member is not an array.");
			return;
		}
		for (auto& el : member->value.GetArray())
		{
			if (el.Is<ValueType>())
			{
				assert(false && "An element in the array is the wrong type.");
				return;
			}
			aCallback(el.Get<ValueType>());
		}
	}

	template<typename ValueType>
	inline ValueType JsonUtility::ParseValue(const char* aMemberName) const
	{
		rapidjson::Value::ConstMemberIterator member = myJsonDocument.FindMember(aMemberName);
		assert((member != myJsonDocument.MemberEnd()) && (aMemberName) && (" not found."));
		assert((member->value.Is<ValueType>()) && aMemberName);
		return member->value.Get<ValueType>();
	}

	template<typename ValueType>
	inline ValueType JsonUtility::ParseValue(const char* aMemberName)
	{
		rapidjson::Value::ConstMemberIterator member = myJsonDocument.FindMember(aMemberName);
		assert((member != myJsonDocument.MemberEnd()) && (aMemberName) && (" not found."));
		assert((member->value.Is<ValueType>()) && aMemberName);
		return member->value.Get<ValueType>();
	}

	template<class ValueType>
	inline void JsonUtility::WriteValue(const char* aMemberName,const ValueType& aValue)
	{
		auto member = myJsonDocument.FindMember(aMemberName);
		if (member != myJsonDocument.MemberEnd())
		{
			member->value.Set<ValueType>(aValue, myJsonDocument.GetAllocator());
		}
		else
		{
			myJsonDocument.AddMember(rapidjson::StringRef(aMemberName), rapidjson::Value(aValue), myJsonDocument.GetAllocator());
		}
	}
	template<>
	inline void JsonUtility::WriteValue(const char* aMemberName, const std::string& aValue)
	{
		auto member = myJsonDocument.FindMember(aMemberName);
		if (member != myJsonDocument.MemberEnd())
		{
			member->value.SetString(rapidjson::StringRef(aValue.c_str(), aValue.size()),  myJsonDocument.GetAllocator());
		}
		else
		{
			rapidjson::Value value;
			value.SetString(aValue.c_str(), static_cast<rapidjson::SizeType>(aValue.size()), myJsonDocument.GetAllocator());
			myJsonDocument.AddMember(rapidjson::StringRef(aMemberName), value, myJsonDocument.GetAllocator());
		}
	}
	
#pragma region StringSpecializations
	template<size_t N>
	inline void JsonUtility::WriteValue(const char* aMemberName, const char(&aValue)[N])
	{
		WriteValue(aMemberName, std::string(aValue));
	}
	template<>
	inline void JsonUtility::WriteValue(const char* aMemberName, const char* const& aValue)
	{
		WriteValue(aMemberName, std::string(aValue));
	}
#pragma endregion
	inline void JsonUtility::EraseValue(const char* aMemberName)
	{
		myJsonDocument.EraseMember(aMemberName);
	}
}

