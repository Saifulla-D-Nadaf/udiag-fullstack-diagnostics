// cloud/dashboard_web/src/App.js
import React, { useState, useEffect } from 'react';
import io from 'socket.io-client';

const App = () => {
  const [faults, setFaults] = useState([]);

  useEffect(() => {
    const socket = io('http://localhost:3000'); // From Node.js API
    socket.on('fault_update', (data) => {
      setFaults(prev => [data, ...prev].slice(0, 50));
    });

    return () => socket.disconnect();
  }, []);

  const getFaultString = (flags) => {
    const f = [];
    if (flags & 1) f.push("Temp High");
    if (flags & 2) f.push("Temp Low");
    if (flags & 4) f.push("Volt High");
    if (flags & 8) f.push("Volt Low");
    return f.join(", ") || "None";
  };

  return (
    <div style={{ padding: 20 }}>
      <h1>µDiag - Real-Time Fault Monitor</h1>
      <table border="1" cellPadding="8">
        <thead>
          <tr>
            <th>Time</th>
            <th>Faults</th>
          </tr>
        </thead>
        <tbody>
          {faults.map((f, i) => (
            <tr key={i}>
              <td>{new Date(f.ts).toLocaleTimeString()}</td>
              <td>{getFaultString(f.flags)}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};

export default App;