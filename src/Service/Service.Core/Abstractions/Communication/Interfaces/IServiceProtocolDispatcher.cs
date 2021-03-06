﻿using Service.Core.Abstractions.Communication.Structures;
using System;
using System.Collections.Generic;
using System.Text;
using static Service.Core.Abstractions.Communication.Interfaces.IServiceProtocolDispatcher;

namespace Service.Core.Abstractions.Communication.Interfaces
{
    /// <summary>
    /// Implements methods to handle client/server protocol.
    /// It is responsible for authentication and session management
    /// </summary>
    public interface IServiceProtocolDispatcher
    {

        ///Dispatch messages received from clients to the server.
        public DispatchResult DispatchClientRequest(byte[] inputBytes);

        public byte[] BuildClientResponse(IExecutionResult executionResult);
    }


}
