using System;

using Azteck;

namespace Sandbox
{
	public class Player : Entity
	{
		public float Speed = 1.0f;
		public float Time = 0.0f;

		private TransformComponent _transform;
		private Rigidbody2DComponent _rigidbody;

		void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate - {ID}");

			_transform = GetComponent<TransformComponent>();
			_rigidbody = GetComponent<Rigidbody2DComponent>();
		}

		void OnUpdate(float ts)
		{
			Time += ts;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.W))
				velocity.Y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.Y = -1.0f;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.X = 1.0f;

			velocity *= Speed * ts;

			_rigidbody.ApplyLinearImpulse(velocity.XY, true);

			//Vector3 translation = m_Transform.Translation;
			//translation += velocity * ts;
			//m_Transform.Translation = translation;
		}

	}
}
