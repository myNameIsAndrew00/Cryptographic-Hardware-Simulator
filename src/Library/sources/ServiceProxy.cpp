#include "../include/ServiceProxy.h"

using namespace Abstractions;

ServiceProxy::ServiceProxy(
	const IServiceCommunicationResolverReference& communicationResolver,
	const IServiceProtocolDispatcherReference& protocolDispatcher
	)
	: communicationInitialised(false)
{ 
	this->communicationResolver = communicationResolver;
	this->protocolDispatcher = protocolDispatcher;
}

bool ServiceProxy::Register(const IServiceProxyClientReference& client) {

	this->client = client; 

	if (this->communicationInitialised)
		this->communicationResolver->FinaliseCurrentCommunication();

	this->communicationInitialised = this->communicationResolver->InitialiseCommunication();

	return this->communicationInitialised;
}

bool ServiceProxy::DetachCurrentClient() {
	if (!this->communicationInitialised) return true;

	return this->communicationResolver->FinaliseCurrentCommunication();
}

Abstractions::CreateSessionResult Abstractions::ServiceProxy::BeginSession()
{  
	unsigned long resultCode = (unsigned long)Abstractions::CreateSessionResult::Code::OkResult;

	BytesReader* reader = this->executeRequest(Abstractions::ServiceActionCode::BeginSession, resultCode, nullptr, 0);
	if (reader == nullptr) return CreateSessionResult(Abstractions::CreateSessionResult::Code::OkResult, -1);

	unsigned char result = reader->PeekChar();
	
	delete reader;
	return CreateSessionResult(Abstractions::CreateSessionResult::Code::OkResult, result);
}

EndSessionResult Abstractions::ServiceProxy::EndSession(const unsigned char sessionId)
{
	unsigned long resultCode = 0;
	BytesReader* reader = this->executeRequest(Abstractions::ServiceActionCode::EndSession, resultCode, &sessionId, sizeof(const unsigned char));

	//todo: change response to use resultCode
	if (reader == nullptr) return EndSessionResult(Abstractions::EndSessionResult::Code::OkResult, false);

	delete reader;
	return EndSessionResult(Abstractions::EndSessionResult::Code::OkResult, true);
}



#pragma region Private

BytesReader* ServiceProxy::executeRequest(Abstractions::ServiceActionCode serviceActionCode, unsigned long& resultCode, const unsigned char* data, const unsigned int dataLength) {
	if (!this->communicationInitialised) return nullptr;

	Bytes clientRequest = data == nullptr ?
		this->protocolDispatcher->CreateClientRequest(serviceActionCode) :
		this->protocolDispatcher->CreateClientRequest(serviceActionCode, Bytes(data, dataLength));

	Abstractions::ServiceExecutionResult executionResult =
		this->protocolDispatcher->ParseServiceResponse(
			this->communicationResolver->ExecuteRequest(clientRequest));

	resultCode = executionResult.GetResultCode();
	auto reader = new Abstractions::BytesReader(BytesReference(new Bytes(std::move(executionResult.GetBytes()))));
	 
	return reader;	
} 


#pragma endregion