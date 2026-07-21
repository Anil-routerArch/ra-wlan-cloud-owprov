//
// Created by OpenWifi on 2026-07-21.
//

#include "framework/RESTAPI_Middleware.h"

#include "framework/RESTAPI_Handler.h"

namespace OpenWifi {

	RESTAPIMiddlewareResult RESTAPIMiddlewareResult::Allow() {
		return RESTAPIMiddlewareResult(true, "");
	}

	RESTAPIMiddlewareResult RESTAPIMiddlewareResult::Deny(std::string Reason) {
		return RESTAPIMiddlewareResult(false, std::move(Reason));
	}

	void RESTAPIMiddlewarePipeline::Use(std::unique_ptr<RESTAPIMiddleware> Middleware) {
		Middleware_.push_back(std::move(Middleware));
	}

	RESTAPIMiddlewareResult RESTAPIMiddlewarePipeline::Execute(
		RESTAPIHandler &Handler,
		const RESTAPIRequestContext &Context) const {
		for (const auto &Middleware : Middleware_) {
			auto Result = Middleware->Process(Handler, Context);
			if (!Result.Allowed()) {
				return Result;
			}
		}
		return RESTAPIMiddlewareResult::Allow();
	}

	RESTAPIMiddlewareResult RESTAPIAuthorizationMiddleware::Process(
		RESTAPIHandler &Handler,
		const RESTAPIRequestContext &Context) {
		std::string Reason;
		if (!Handler.RoleIsAuthorized(Context.Path(), Context.Method(), Reason)) {
			return RESTAPIMiddlewareResult::Deny(Reason);
		}
		return RESTAPIMiddlewareResult::Allow();
	}

} // namespace OpenWifi
