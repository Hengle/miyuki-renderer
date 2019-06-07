#ifndef MIYUKI_REFLECTION_ARRAY_HPP
#define MIYUKI_REFLECTION_ARRAY_HPP

#include "object.hpp"
#include "primitive.hpp"

namespace Miyuki {
	namespace Reflection {
		template<class T>
		class Array final : public Object {
			using Ty = typename _ConvertToPrimitiveType<T>::type;
			std::vector<Ty*> _array;
		public:
			static Class* __classinfo__() {
				static Class* info = nullptr;
				static std::once_flag flag;
				std::call_once(flag, [&]() {
					info = new Class();
					auto s = new std::string("Miyuki::Reflection::Array<");
					*s += Ty::__classinfo__()->name();
					*s += ">";
					info->_name = s->c_str();
					info->classInfo.base = Object::__classinfo__();
					info->classInfo.ctor = [=](const std::string& n) {return new Array<T>(n); };
					});
				return info;
			}
			Array(const std::string& name = "") :Object(__classinfo__(), name) {}
			void push_back(Ty* t) { _array.push_back(t); }
			void pop_back() { _array.pop_back(); }
			auto begin()const { return _array.begin(); }
			auto begin() { return _array.begin(); }
			auto end()const { return _array.end(); }
			auto end() { return _array.end(); }
			void serialize(json& j, SerializationState& state)const override {
				Object::serialize(j, state);
				j["array"] = json::array();
				for (const auto& i : _array) {
					json tmp;
					i->serialize(tmp, state);
					j["array"].push_back(tmp);
				}
			}
		};
	}
}
#endif