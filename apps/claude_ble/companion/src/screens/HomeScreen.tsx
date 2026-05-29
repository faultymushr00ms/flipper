import React, { useState, useCallback, useRef } from 'react';
import {
  ActivityIndicator,
  ScrollView,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from 'react-native';
import { useBle } from '../ble/useBle';
import { askClaude } from '../api/claude';

export function HomeScreen() {
  const [lastPrompt, setLastPrompt] = useState('');
  const [response,   setResponse]   = useState('');
  const [loading,    setLoading]    = useState(false);
  const [error,      setError]      = useState('');

  // Stable ref so onPromptReceived can call sendResponse without stale closure
  const sendResponseRef = useRef<((text: string) => Promise<void>) | null>(null);

  const onPromptReceived = useCallback(async (prompt: string) => {
    setLastPrompt(prompt);
    setResponse('');
    setError('');
    setLoading(true);
    try {
      const reply = await askClaude(prompt);
      setResponse(reply);
      await sendResponseRef.current?.(reply);
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Unknown error');
    } finally {
      setLoading(false);
    }
  }, []);

  const { status, connect, sendResponse } = useBle(onPromptReceived);
  sendResponseRef.current = sendResponse;

  const dotColor =
    status === 'connected' ? '#22c55e' :
    status === 'error'     ? '#ef4444' : '#94a3b8';

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Claude BLE Bridge</Text>

      <View style={styles.statusRow}>
        <View style={[styles.dot, { backgroundColor: dotColor }]} />
        <Text style={styles.statusText}>{status}</Text>
      </View>

      {(status === 'idle' || status === 'error') && (
        <TouchableOpacity style={styles.button} onPress={connect}>
          <Text style={styles.buttonText}>
            {status === 'error' ? 'Retry Connection' : 'Connect to Flipper'}
          </Text>
        </TouchableOpacity>
      )}

      {status === 'scanning' && (
        <ActivityIndicator style={styles.spinner} color="#334155" />
      )}

      {lastPrompt ? (
        <View style={styles.card}>
          <Text style={styles.cardLabel}>PROMPT</Text>
          <Text style={styles.cardBody}>{lastPrompt}</Text>
        </View>
      ) : null}

      {loading && <ActivityIndicator style={styles.spinner} color="#334155" />}

      {error ? (
        <View style={[styles.card, styles.cardError]}>
          <Text style={styles.cardLabel}>ERROR</Text>
          <Text style={styles.cardBody}>{error}</Text>
        </View>
      ) : null}

      {response && !loading ? (
        <ScrollView style={styles.card}>
          <Text style={styles.cardLabel}>RESPONSE</Text>
          <Text style={styles.cardBody}>{response}</Text>
        </ScrollView>
      ) : null}
    </View>
  );
}

const styles = StyleSheet.create({
  container:  { flex: 1, padding: 20 },
  title:      { fontSize: 22, fontWeight: '700', marginBottom: 12 },
  statusRow:  { flexDirection: 'row', alignItems: 'center', marginBottom: 16 },
  dot:        { width: 10, height: 10, borderRadius: 5, marginRight: 8 },
  statusText: { fontSize: 14, color: '#64748b' },
  button:     { backgroundColor: '#1e293b', padding: 14, borderRadius: 8, alignItems: 'center', marginBottom: 16 },
  buttonText: { color: '#fff', fontSize: 16, fontWeight: '600' },
  spinner:    { marginVertical: 16 },
  card:       { backgroundColor: '#f8fafc', borderRadius: 8, padding: 12, marginTop: 12, maxHeight: 220 },
  cardError:  { backgroundColor: '#fef2f2' },
  cardLabel:  { fontSize: 10, fontWeight: '700', color: '#94a3b8', marginBottom: 4, letterSpacing: 1 },
  cardBody:   { fontSize: 15, color: '#1e293b', lineHeight: 22 },
});
