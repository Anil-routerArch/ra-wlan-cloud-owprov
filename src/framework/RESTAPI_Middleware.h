//
// Created by OpenWifi on 2026-07-21.
//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Poco/JSON/Object.h"

#include "RESTObjects/RESTAPI_SecurityObjects.h"

namespace OpenWifi {

	class RESTAPIHandler;

	class RESTAPIRequestContext {
	  public:
		RESTAPIRequestContext(std::string Path,
							  std::string Method,
							  const SecurityObjects::UserInfoAndPolicy &User,
							  Poco::JSON::Object::Ptr ParsedBody)
			: Path_(std::move(Path)), Method_(std::move(Method)), User_(User),
			  ParsedBody_(std::move(ParsedBody)) {}

		[[nodiscard]] const std::string &Path() const { return Path_; }
		[[nodiscard]] const std::string &Method() const { return Method_; }
		[[nodiscard]] const SecurityObjects::UserInfoAndPolicy &User() const { return User_; }
		[[nodiscard]] const Poco::JSON::Object::Ptr &ParsedBody() const { return ParsedBody_; }

	  private:
		std::string Path_;
		std::string Method_;
		const SecurityObjects::UserInfoAndPolicy &User_;
		Poco::JSON::Object::Ptr ParsedBody_;
	};

	class RESTAPIMiddlewareResult {
	  public:
		static RESTAPIMiddlewareResult Allow();
		static RESTAPIMiddlewareResult Deny(std::string Reason);

		[[nodiscard]] bool Allowed() const { return Allowed_; }
		[[nodiscard]] const std::string &Reason() const { return Reason_; }

	  private:
		RESTAPIMiddlewareResult(bool Allowed, std::string Reason)
			: Allowed_(Allowed), Reason_(std::move(Reason)) {}

		bool Allowed_ = true;
		std::string Reason_;
	};

	class RESTAPIMiddleware {
	  public:
		virtual ~RESTAPIMiddleware() = default;

		virtual RESTAPIMiddlewareResult Process(RESTAPIHandler &Handler,
												const RESTAPIRequestContext &Context) = 0;
	};

	class RESTAPIMiddlewarePipeline {
	  public:
		void Use(std::unique_ptr<RESTAPIMiddleware> Middleware);
		RESTAPIMiddlewareResult Execute(RESTAPIHandler &Handler,
										const RESTAPIRequestContext &Context) const;

	  private:
		std::vector<std::unique_ptr<RESTAPIMiddleware>> Middleware_;
	};

	class RESTAPIAuthorizationMiddleware : public RESTAPIMiddleware {
	  public:
		RESTAPIMiddlewareResult Process(RESTAPIHandler &Handler,
										const RESTAPIRequestContext &Context) override;
	};

} // namespace OpenWifi
