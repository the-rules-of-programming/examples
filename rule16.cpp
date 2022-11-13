
#include <vector>
#include <unordered_map>
#include <set>
#include <assert.h>
#include <ctype.h>
#include "tests.h"

using namespace std;

namespace rule16
{
	namespace Json
	{
		class Value;

		class Stream
		{
		public:

			Stream(FILE * file);

			int peekChar();
			int getChar();

		protected:

			FILE * m_file;
			int m_nextChar;
		};

		Stream::Stream(FILE * file) :
			m_file(file),
			m_nextChar(EOF)
		{
		}

		int Stream::peekChar()
		{
			if (m_nextChar == EOF)
				m_nextChar = fgetc(m_file);

			return m_nextChar;
		}

		int Stream::getChar()
		{
			if (m_nextChar != EOF)
			{
				int nextChar = m_nextChar;
				m_nextChar = EOF;

				return nextChar;
			}

			return fgetc(m_file);
		}

		struct Object
		{
			unordered_map<string, Value> m_values;
		};

		struct Array
		{
			vector<Value> m_values;
		};

		class Value
		{
		public:

			Value() :
				m_type(Type::Null), 
				m_str(), 
				m_number(0.0), 
				m_object(), 
				m_array()
				{ ; }

			bool isString() const
				{ return m_type == Type::String; }
			bool isNumber() const
				{ return m_type == Type::Number; }
			bool isObject() const
				{ return m_type == Type::Object; }
			bool isArray() const
				{ return m_type == Type::Array; }
			bool isTrue() const
				{ return m_type == Type::True; }
			bool isFalse() const
				{ return m_type == Type::False; }
			bool isNull() const
				{ return m_type == Type::Null; }

			operator const string & () const
				{ assert(isString()); return m_str; }
			operator double () const
				{ assert(isNumber()); return m_number; }
			operator const Object & () const
				{ assert(isObject()); return m_object; }
			operator const Array & () const
				{ assert(isArray()); return m_array; }

			void format(int indent) const;

			static bool tryReadValue(Stream * stream, Value * value);

		protected:

			enum class Type
			{
				String,
				Number,
				Object,
				Array,
				True,
				False,
				Null
			};

			static const char * getIndentString(int indent);
			static void formatString(const string & str);

			static bool tryReadObject(Stream * stream, Object * object);
			static bool tryReadArray(Stream * stream, Array * array);
			static bool tryReadString(Stream * stream, string * str);
			static bool tryReadNumber(Stream * stream, double * number);
			static bool tryReadLiteral(Stream * stream, const char * literal);
			static void skipWhiteSpace(Stream * stream);

			Type m_type;

			string m_str;
			double m_number;
			Object m_object;
			Array m_array;
		};

		const char * Value::getIndentString(int indent)
		{
			static char s_blanks[] = "                                                            ";
			return &s_blanks[max(0, int(sizeof(s_blanks)) - indent - 1)];
		}

		void Value::formatString(const string & str)
		{
			// Early exit if the string requires no escape sequences

			for (const char * nextChar = str.c_str();; ++nextChar)
			{
				if (!*nextChar)
				{
					printf("\"%s\"", str.c_str());
					return;
				}

				if (*nextChar < 0x20 || *nextChar == '\\' || *nextChar == '"')
					break;
			}

			// Build string with escape sequences

			string escapeString;

			for (const char * nextChar = str.c_str(); *nextChar; ++nextChar)
			{
				switch (*nextChar)
				{
				case 8:
					escapeString.append("\\b");
					break;

				case 9:
					escapeString.append("\\t");
					break;

				case 10:
					escapeString.append("\\l");
					break;

				case 12:
					escapeString.append("\\f");
					break;

				case 13:
					escapeString.append("\\r");
					break;

				case '"':
					escapeString.append("\\\"");
					break;

				case '\\':
					escapeString.append("\\\\");
					break;

				default:
					{
						if (*nextChar < 0x20)
						{
							char buffer[8];
							sprintf_s(buffer, "u%04x", *nextChar);
							escapeString.append(buffer);
						}
						else
						{
							escapeString.push_back(*nextChar);
						}
					}
					break;
				}
			}

			printf("\"%s\"", escapeString.c_str());
		}

		void Value::format(int indent) const
		{
			switch (m_type)
			{
			case Type::String:
				formatString(m_str);
				break;

			case Type::Number:
				printf("%g", m_number);
				break;

			case Type::Object:
				{
					char separator = '{';
					for (const auto & value : m_object.m_values)
					{
						printf("%c\n%s\"%s\" : ", separator, getIndentString(indent + 2), value.first.c_str());
						value.second.format(indent + 2);
						separator = ',';
					}
					printf("\n%s}", getIndentString(indent));
				}
				break;

			case Type::Array:
				{
					char separator = '[';
					for (const auto & value : m_array.m_values)
					{
						printf("%c\n%s", separator, getIndentString(indent + 2));
						value.format(indent + 2);
						separator = ',';
					}
					printf("\n%s]", getIndentString(indent));
				}
				break;

			case Type::True:
				printf("\"true\"");
				break;

			case Type::False:
				printf("\"false\"");
				break;

			case Type::Null:
				printf("\"null\"");
				break;
			}
		}

		bool Value::tryReadValue(Stream * stream, Value * value)
		{
			skipWhiteSpace(stream);

			int nextChar = stream->peekChar();
			switch (nextChar)
			{
			case '{':
				{
					value->m_type = Type::Object;
					if (!Value::tryReadObject(stream, &value->m_object))
						return false;
				}
				break;

			case '[':
				{
					value->m_type = Type::Array;
					if (!Value::tryReadArray(stream, &value->m_array))
						return false;
				}
				break;

			case '"':
				{
					string str;
					if (!Value::tryReadString(stream, &str))
						return false;

					if (str == "true")
					{
						value->m_type = Type::True;
					}
					else if (str == "false")
					{
						value->m_type = Type::False;
					}
					else if (str == "null")
					{
						value->m_type = Type::Null;
					}
					else
					{
						value->m_type = Type::String;
						value->m_str = str;
					}
				}
				break;

			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					value->m_type = Type::Number;
					if (!Value::tryReadNumber(stream, &value->m_number))
						return false;
				}
				break;

			case 't':
				{
					if (!Value::tryReadLiteral(stream, "true"))
						return false;

					value->m_type = Type::True;
				}
				break;

			case 'f':
				{
					if (!Value::tryReadLiteral(stream, "false"))
						return false;

					value->m_type = Type::False;
				}
				break;

			case 'n':
				{
					if (!Value::tryReadLiteral(stream, "null"))
						return false;

					value->m_type = Type::Null;
				}
				break;

			default:
				return false;
			}

			skipWhiteSpace(stream);
			return true;
		}

		bool Value::tryReadObject(Stream * stream, Object * object)
		{
			int nextChar = stream->getChar();
			if (nextChar != '{')
				return false;

			skipWhiteSpace(stream);

			if (stream->peekChar() == '}')
				return true;

			while (true)
			{
				if (stream->peekChar() != '"')
					return false;

				string str;
				if (!tryReadString(stream, &str))
					return false;

				skipWhiteSpace(stream);

				nextChar = stream->getChar();
				if (nextChar != ':')
					return false;

				Value value;
				if (!tryReadValue(stream, &value))
					return false;

				object->m_values[str] = value;

				nextChar = stream->getChar();
				if (nextChar == '}')
					return true;
				else if (nextChar != ',')
					return false;

				skipWhiteSpace(stream);
			}
		}

		bool Value::tryReadArray(Stream * stream, Array * array)
		{
			int nextChar = stream->getChar();
			if (nextChar != '[')
				return false;

			skipWhiteSpace(stream);

			if (stream->peekChar() == ']')
				return true;

			while (true)
			{
				Value value;
				if (!tryReadValue(stream, &value))
					return false;

				array->m_values.push_back(value);

				nextChar = stream->getChar();
				if (nextChar == ']')
					return true;
				else if (nextChar != ',')
					return false;
			}
		}

		bool Value::tryReadString(Stream * stream, string * str)
		{
			int nextChar = stream->getChar();
			if (nextChar != '"')
				return false;

			while (true)
			{
				nextChar = stream->getChar();
				if (nextChar == '"')
				{
					return true;
				}
				else if (nextChar == EOF)
				{
					return false;
				}
				else if (nextChar == '\\')
				{
					nextChar = stream->getChar();
					switch (nextChar)
					{
					case '"':
					case '\\':
					case '/':
						break;

					case 'b':
						nextChar = 8;
						break;

					case 't':
						nextChar = 9;
						break;

					case 'n':
						nextChar = 10;
						break;

					case 'f':
						nextChar = 12;
						break;

					case 'r':
						nextChar = 13;
						break;

					case 'u':
						{
							int hexValue = 0;

							for (int i = 0; i < 4; ++i)
							{
								nextChar = stream->getChar();
								int hexDigit = 0;

								if (nextChar >= '0' && nextChar <= '9')
									hexDigit = (nextChar - '0');
								else if (nextChar >= 'a' && nextChar <= 'f')
									hexDigit = 10 + (nextChar - 'a');
								else if (nextChar >= 'A' && nextChar <= 'F')
									hexDigit = 10 + (nextChar - 'A');
								else
									return false;

								hexValue = hexValue * 16 + hexDigit;
							}

							if (hexValue >= 256)
								return false;

							nextChar = hexValue;
						}
						break;

					default:
						return false;
					}
				}

				str->push_back(char(nextChar));
			}
		}

		bool Value::tryReadNumber(Stream * stream, double * number)
		{
			//

			double sign = 1.0;

			int nextChar = stream->peekChar();
			if (nextChar == '-')
			{
				stream->getChar();
				sign = -1.0;
			}

			//

			double integerPart = 0.0;

			while (true)
			{
				nextChar = stream->peekChar();
				if (nextChar >= '0' && nextChar <= '9')
				{
					stream->getChar();
					integerPart = integerPart * 10.0 + (nextChar - '0');
				}
				else if (nextChar == '.' || nextChar == 'e' || nextChar == 'E')
				{
					break;
				}
				else
				{
					*number = sign * integerPart;
					return true;
				}
			}

			assert(nextChar == '.' || nextChar == 'e' || nextChar == 'E');

			//

			double fractionalPart = 0.0;
			double fractionalMultiplier = 0.1;

			if (nextChar == '.')
			{
				(void) stream->getChar();

				while (true)
				{
					nextChar = stream->peekChar();
					if (nextChar >= '0' && nextChar <= '9')
					{
						stream->getChar();
						fractionalPart = fractionalPart + fractionalMultiplier * (nextChar - '0');
						fractionalMultiplier /= 10.0;
					}
					else if (nextChar == 'e' || nextChar == 'E')
					{
						break;
					}
					else
					{
						*number = sign * (integerPart + fractionalPart);
						return true;
					}
				}
			}

			assert(nextChar == 'e' || nextChar == 'E');

			//

			double exponentSign = 1.0;
			double exponentPart = 0.0;

			(void) stream->getChar();

			nextChar = stream->peekChar();
			if (nextChar == '-')
			{
				stream->getChar();
				exponentSign = -1.0;
			}

			while (true)
			{
				nextChar = stream->peekChar();
				if (nextChar >= '0' && nextChar <= '9')
				{
					stream->getChar();
					integerPart = integerPart * 10.0 + double(nextChar - '0');
				}
				else
				{
					*number = sign * (integerPart + fractionalPart) * pow(10.0, exponentSign * exponentPart);
					return true;
				}
			}
		}

		bool Value::tryReadLiteral(Stream * stream, const char * literal)
		{
			for (const char * nextChar = literal; *nextChar; ++nextChar)
			{
				if (stream->getChar() != *nextChar)
					return false;
			}

			return true;
		}

		void Value::skipWhiteSpace(Stream * stream)
		{
			while (isspace(stream->peekChar()))
			{
				stream->getChar();
			}
		}
	};

	namespace misc
	{
		vector<string> splitString(
			string str,
			const char * separators)
		{
			vector<string> parts;
			const char * firstChar = &str[0];

			while (true)
			{
				const char * lastChar = firstChar + strcspn(firstChar, separators);

				parts.push_back(string(firstChar, lastChar - firstChar));

				if (!*lastChar)
					break;

				firstChar = lastChar + 1;
			}

			return parts;
		}

		template <class T>
		string joinString(
			T begin,
			T end,
			const char * separators)
		{
			string str;
			for (T cur = begin; cur != end; ++cur)
			{
				if (cur != begin)
					str.push_back(separators[0]);

				str.append(*cur);
			}

			return str;
		}

		void log(const char * str, ...)
		{
		}
	};

	namespace cfg
	{
		using namespace Json;

		Value g_config;
	};

    namespace x1
	{
		using namespace cfg;
		using namespace misc;

		bool isServerBlocked(string server)
		{
			if (!g_config.isObject())
			{
				log("expected object for config");
				return false;
			}

			const Object & configObject = g_config;
			const auto & findSecurity = configObject.m_values.find("security");
			if (findSecurity == configObject.m_values.end())
				return false;

			if (!findSecurity->second.isObject())
			{
				log("expected object for config.security");
				return false;
			}

			const Object & securityObject = findSecurity->second;
			const auto & find = securityObject.m_values.find("blocked_servers");
			if (find == securityObject.m_values.end())
				return false;

			if (!find->second.isArray())
			{
				log("expected string array for config.security.blocked_servers");
				return false;
			}

			const Array & blockedServersArray = find->second;
			for (const Value & value : blockedServersArray.m_values)
			{
				if (!value.isString())
				{
					log("expect string array for config.security.blocked_servers");
					continue;
				}

				const string & blockedServer = value;
				if (blockedServer == server)
					return true;
			}

			return false;
		}
	};

	namespace ekp
	{
		using namespace Json;
		using namespace misc;

		const Value * evaluateKeyPath(const Value & rootValue, string keyPath)
		{
			vector<string> keys = splitString(keyPath, ".");

			const Value * currentValue = &rootValue;
			for (unsigned int keyIndex = 0; keyIndex < keys.size(); ++keyIndex)
			{
				if (!currentValue->isObject())
				{
					log(
						"expected %s to be an object", 
						joinString(&keys[0], &keys[keyIndex + 1], ".").c_str());
					return nullptr;
				}

				const Object & object = *currentValue;
				const auto & findKey = object.m_values.find(keys[keyIndex]);
				if (findKey == object.m_values.end())
					return nullptr;

				currentValue = &findKey->second;
			}

			return currentValue;
		}

		const Array * evaluateKeyPathToArray(const Value & rootValue, string keyPath)
		{
			const Value * value = evaluateKeyPath(rootValue, keyPath);
			if (!value)
				return nullptr;

			if (!value->isArray())
			{
				log("expected %s to be an Array", keyPath);
				return nullptr;
			}

			return &static_cast<const Array &>(*value);
		}
	};

	namespace x2
	{
		using namespace cfg;
		using namespace ekp;

		bool isServerBlocked(string server)
		{
			const Value * value = evaluateKeyPath(
									g_config, 
									"security.blocked_servers");
			if (!value)
				return false;

			if (!value->isArray())
			{
				log("expected string array for security.blocked_servers");
				return false;
			}

			const Array & blockedServersArray = *value;
			for (const Value & value : blockedServersArray.m_values)
			{
				if (!value.isString())
				{
					log("expected string array for security.blocked_servers");
					continue;
				}

				const string & blockedServer = value;
				if (blockedServer == server)
					return true;
			}

			return false;
		}
	};

	namespace x3
	{
		using namespace cfg;
		using namespace ekp;

		bool isServerBlocked(string server)
		{
			const Array * array = evaluateKeyPathToArray(
										g_config, 
										"security.blocked_servers");
			if (!array)
				return false;

			for (const Value & value : array->m_values)
			{
				if (!value.isString())
				{
					log("expected string array for security.blocked_servers");
					continue;
				}

				const string & blockedServer = value;
				if (blockedServer == server)
					return true;
			}

			return false;
		}
	};

	namespace x4
	{
		using namespace Json;
		using namespace ekp;

		struct Config
		{
			set<string> m_blockedServers;
		};
		Config g_config;

		bool isServerBlocked(string serverURL)
		{
			return (g_config.m_blockedServers.count(serverURL) > 0);
		}

		void unpackStringArray(
			const Value & value,
			const char * keyPath, 
			set<string> * strings)
		{
			const Array * array = evaluateKeyPathToArray(value, keyPath);
			if (array)
			{
				for (const Value & valueString : array->m_values)
				{
					if (!valueString.isString())
					{
						log("expected %s to be an array of strings", keyPath);
					}

					strings->emplace(valueString);
				}
			}
		}

		void unpackConfig(const Value & value, Config * config)
		{
			unpackStringArray(
				value,
				"security.blocked_servers", 
				&config->m_blockedServers);
		}
	};

	namespace x5
	{
		using namespace Json;
		using namespace misc;

		// Type safety

		struct UnsafeType
		{
		protected:

			template <typename T> friend struct StructType;
			virtual bool tryUnpack(const Value & value, void * data) const = 0;
		};

		template <class T>
		struct SafeType : public UnsafeType
		{
			virtual bool tryUnpack(const Value & value, T * data) const = 0;

		protected:

			virtual bool tryUnpack(const Value & value, void * data) const override
				{ return tryUnpack(value, static_cast<T *>(data));  }
		};

		// Simple types

		struct BoolType : public SafeType<bool>
		{
			bool tryUnpack(const Value & value, bool * data) const override;
		};

		struct IntegerType : public SafeType<int>
		{
			bool tryUnpack(const Value & value, int * data) const override;
		};

		struct DoubleType : public SafeType<double>
		{
			bool tryUnpack(const Value & value, double * data) const override;
		};

		struct StringType : public SafeType<string>
		{
			bool tryUnpack(const Value & value, string * data) const override;
		};

		bool BoolType::tryUnpack(const Value & value, bool * data) const
		{
			if (value.isTrue())
			{
				*data = true;
				return true;
			}
			else if (value.isFalse())
			{
				*data = false;
				return true;
			}
			else
			{
				log("expected true or false");
				return false;
			}
		}

		bool IntegerType::tryUnpack(const Value & value, int * data) const
		{
			if (!value.isNumber())
			{
				log("expected number");
				return false;
			}

			double number = value;
			if (number != int(number))
			{
				log("expected integer");
				return false;
			}

			*data = int(number);
			return true;
		}

		bool DoubleType::tryUnpack(const Value & value, double * data) const
		{
			if (!value.isNumber())
			{
				log("expected number");
				return false;
			}

			*data = value;
			return true;
		}

		bool StringType::tryUnpack(const Value & value, string * data) const
		{
			if (!value.isString())
			{
				log("expected string");
				return false;
			}

			*data = static_cast<const string &>(value);
			return true;
		}

		// Vector type

		template <class T>
		struct VectorType : public SafeType<vector<T>>
		{
			VectorType(SafeType<T> * elementType);
			bool tryUnpack(const Value & value, vector<T> * data) const override;

			SafeType<T> * m_elementType;
		};

		template <class T>
		VectorType<T>::VectorType(SafeType<T> * elementType) :
			m_elementType(elementType)
		{
		}

		template <class T>
		bool VectorType<T>::tryUnpack(const Value & value, vector<T> * data) const
		{
			if (!value.isArray())
			{
				log("expected array");
				return false;
			}

			const Array & array = value;
			for (const Value & arrayValue : array.m_values)
			{
				T t;
				if (!m_elementType->tryUnpack(arrayValue, &t))
					return false;

				data->push_back(t);
			}

			return true;
		}

		// Set type

		template <class T>
		struct SetType : public SafeType<set<T>>
		{
			SetType(SafeType<T> * elementType);
			bool tryUnpack(const Value & value, set<T> * data) const override;

			SafeType<T> * m_elementType;
		};

		template <class T>
		SetType<T>::SetType(SafeType<T> * elementType) :
			m_elementType(elementType)
		{
		}

		template <class T>
		bool SetType<T>::tryUnpack(const Value & value, set<T> * data) const
		{
			if (!value.isArray())
			{
				log("expected array");
				return false;
			}

			const Array & array = value;
			for (const Value & arrayValue : array.m_values)
			{
				T t;
				if (!m_elementType->tryUnpack(arrayValue, &t))
					return false;

				data->emplace(t);
			}

			return true;
		}

		// Fields

		template <class S>
		struct Field
		{
			template <class T>
			Field(const char * name, SafeType<T> * type, T S:: * member);

			const char * m_name;
			const UnsafeType * m_type;
			int S::* m_member;
		};

		template <class S>
		template <class T>
		Field<S>::Field(const char * name, SafeType<T> * type, T S::* member) :
			m_name(name), 
			m_type(type), 
			m_member(reinterpret_cast<int S::*>(member))
			{ ; }

		// Struct type

		template <class T>
		struct StructType : public SafeType<T>
		{
			template <class... TT>
			StructType(TT... fields);

			bool tryUnpack(const Value & value, T * data) const override;

		protected:

			vector<Field<T>> m_fields;
		};

		template <class T>
		template <class... TT>
		StructType<T>::StructType(TT... fields) :
			m_fields()
		{
			m_fields.insert(m_fields.end(), { fields... });
		}

		template <class T>
		bool StructType<T>::tryUnpack(const Value & value, T * data) const
		{
			if (!value.isObject())
			{
				log("expected object");
				return false;
			}

			const Object & object = value;
			for (const auto & objectValue : object.m_values)
			{
				const Field<T> * match = nullptr;

				for (const Field<T> & field : m_fields)
				{
					if (field.m_name == objectValue.first)
					{
						match = &field;
						break;
					}
				}

				if (!match)
				{
					log("unrecognized option %s", objectValue.first.c_str());
					return false;
				}

				int T::* member = match->m_member;
				int * fieldData = &(data->*member);

				if (!match->m_type->tryUnpack(objectValue.second, fieldData))
					return false;
			}

			return true;
		}

		// Config

		struct Config
		{
			Config() :
				m_security()
				{ ; }

			struct Security
			{
				Security() :
					m_blockedServers()
					{ ; }

				set<string> m_blockedServers;
			};
				
			Security m_security;
		};
		Config g_config;

		StructType<Config::Security> g_securityType(
			Field<Config::Security>(
				"blocked_servers", 
				new SetType<string>(new StringType), 
				&Config::Security::m_blockedServers));

		StructType<Config> g_configType(
			Field<Config>("security", &g_securityType, &Config::m_security));

		// Startup logic
		
		bool tryStartup()
		{
			FILE * file;
			if (fopen_s(&file, "config.json", "r"))
				return false;

			Stream stream(file);
			Value value;

			bool success = Value::tryReadValue(&stream, &value) &&
							g_configType.tryUnpack(value, &g_config);

			fclose(file);
			return success;
		}
	};

	void test(bool showDetails)
	{
		{
			using namespace Json;

			for (const char * fileName : { "test_1.json", "test_2.json", "test_3.json" })
			{
				FILE * file;
				assert(!fopen_s(&file, fileName, "r"));

				Stream stream(file);
				Value value;

				assert(Value::tryReadValue(&stream, &value));
				fclose(file);
			}
		}

		{
			using namespace x1;

			FILE * file;
			assert(!fopen_s(&file, "config.json", "r"));

			Stream stream(file);
			assert(Value::tryReadValue(&stream, &g_config));
			fclose(file);

			assert(isServerBlocked("www.espn.com"));
			assert(isServerBlocked("www.xkcd.com"));
			assert(isServerBlocked("www.brothers-brick.com"));
			assert(!isServerBlocked("www.therulesofprogramming.com"));
		}

		{
			using namespace x2;

			FILE * file;
			assert(!fopen_s(&file, "config.json", "r"));

			Stream stream(file);
			assert(Value::tryReadValue(&stream, &g_config));
			fclose(file);

			assert(isServerBlocked("www.espn.com"));
			assert(isServerBlocked("www.xkcd.com"));
			assert(isServerBlocked("www.brothers-brick.com"));
			assert(!isServerBlocked("www.therulesofprogramming.com"));
		}

		{
			using namespace x3;

			FILE * file;
			assert(!fopen_s(&file, "config.json", "r"));

			Stream stream(file);
			assert(Value::tryReadValue(&stream, &g_config));
			fclose(file);

			assert(isServerBlocked("www.espn.com"));
			assert(isServerBlocked("www.xkcd.com"));
			assert(isServerBlocked("www.brothers-brick.com"));
			assert(!isServerBlocked("www.therulesofprogramming.com"));
		}

		{
			using namespace x4;

			FILE * file;
			assert(!fopen_s(&file, "config.json", "r"));

			Stream stream(file);
			Value value;
			assert(Value::tryReadValue(&stream, &value));
			fclose(file);

			unpackConfig(value, &g_config);

			const set<string> & blockedServers = g_config.m_blockedServers;

			assert(blockedServers.find("www.espn.com") != blockedServers.end());
			assert(blockedServers.find("www.xkcd.com") != blockedServers.end());
			assert(blockedServers.find("www.brothers-brick.com") != blockedServers.end());
			assert(blockedServers.find("www.therulesofprogramming.com") == blockedServers.end());
		}

		{
			using namespace x5;

			assert(tryStartup());

			const set<string> & blockedServers = g_config.m_security.m_blockedServers;

			assert(blockedServers.find("www.espn.com") != blockedServers.end());
			assert(blockedServers.find("www.xkcd.com") != blockedServers.end());
			assert(blockedServers.find("www.brothers-brick.com") != blockedServers.end());
			assert(blockedServers.find("www.therulesofprogramming.com") == blockedServers.end());
		}
	}

	RegisterTest rt("Rule 16", test);
};
