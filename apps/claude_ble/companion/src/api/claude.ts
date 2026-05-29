import Anthropic from '@anthropic-ai/sdk';
import Config from 'react-native-config';

const client = new Anthropic({ apiKey: Config.ANTHROPIC_API_KEY ?? '' });

// Kept low so the response fits within the Flipper's 512-byte response buffer.
// Raise this together with CLAUDE_BLE_RESPONSE_MAX_LEN in claude_ble_app.h.
const MAX_TOKENS = 150;

export async function askClaude(prompt: string): Promise<string> {
  const message = await client.messages.create({
    model: 'claude-sonnet-4-6',
    max_tokens: MAX_TOKENS,
    messages: [{ role: 'user', content: prompt }],
  });

  const block = message.content[0];
  return block.type === 'text' ? block.text : '';
}
