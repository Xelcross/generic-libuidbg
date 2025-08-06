#ifndef LITE_TOOLS_H
#define LITE_TOOLS_H

#include <memory>
#include <QtCore/QtCore>

namespace lite {
//////////////////////////////从json型数据中读取指定数据////////////////////////////////////
	template <class T, class HashMap>
	bool GetJsonObjectElement(T& result, const QString& key, const QString& name, const QVariant& value);

	template <class T>
	bool GetJsonListElement(T& result, const QString& key, const QString& name, const QVariant& value);
	
	//从json中取值， ${name}: 取值路径，例如：A[2].B[3][2]
	template <class T>
	bool GetJsonValue(T& result, const QString& name, const QVariant& value) {
		bool r(false);
		if (name.isEmpty()) {
			r = true;
			result = value.value<T>();
		}

		QRegExp regexp("\\S(\\.|\\[)");
		auto endIndex = name.indexOf(regexp);
		if (endIndex != -1) ++endIndex;

		const auto key = name.midRef(0, endIndex);
		const auto next = name.midRef(key.size() + (endIndex != -1 && name[endIndex] == '.' ? 1 : 0));
		const auto vType = value.type();

		switch (vType) {
		case QVariant::Map: r = GetJsonObjectElement<T, QVariantMap>(result, key.toString(), next.toString(), value); break;
		case QVariant::Hash: r = GetJsonObjectElement<T, QVariantHash>(result, key.toString(), next.toString(), value); break;
		case QVariant::List: r = GetJsonListElement<T>(result, key.toString(), next.toString(), value); break;
		case QVariant::String: r = GetJsonValue<T>(result, name, value.toByteArray()); break;
		case QVariant::ByteArray: r = GetJsonValue<T>(result, name, value.toByteArray()); break;
		default: break;
		}
		return r;
	}

	template <class T>
	bool GetJsonValue(T& result, const QString& name, const QJsonDocument& document) {
		return GetJsonValue(result, name, document.toVariant());
	}

	template <class T>
	bool GetJsonValue(T& result, const QString& name, const QByteArray& json) {
		return GetJsonValue(result, name, QJsonDocument::fromJson(json).toVariant());
	}

	template <class T>
	bool GetJsonValue(T& result, const QString& name, const QJsonValue& value) {
		return GetJsonValue(result, name, value.toVariant());
	}

	template <class T>
	bool GetJsonListElement(T& result, const QString& key, const QString& name, const QVariant& value) {
		if (!key.startsWith('[') || !key.endsWith(']') || key.size() < 3) return false;
		auto index = key.mid(1, key.size() - 2).toInt();

		const auto& value_ref = reinterpret_cast<const QVariantList&>(value);
		if (index < 0 || index > value_ref.size()) return false;

		if (name.isEmpty()) {
			result = value_ref[index].template value<T>();
			return true;
		} else {
			return GetJsonValue(result, name, value_ref[index]);
		}
	}

	template <class T, class HashMap>
	bool GetJsonObjectElement(T& result, const QString& key, const QString& name, const QVariant& value) {
		const auto& value_ref = reinterpret_cast<const HashMap&>(value);
		if (!value_ref.contains(key)) return false;

		if (name.isEmpty()) {
			result = value_ref[key].template value<T>();
			return true;
		} else {
			return GetJsonValue(result, name, value_ref[key]);
		}
	}
	
//////////////////////////////////////////////////////////////////////////////////////////


	template <class T>
	std::shared_ptr<T> GetGlobalStaticPtr()
	{
		static auto pointer = std::make_shared<T>();
		return pointer;
	}

	template <class _Ty, size_t _Size>
	constexpr size_t ArraySize(const _Ty (&)[_Size]) noexcept {
		return _Size;
	}
}
#endif //LITE_TOOLS_H
